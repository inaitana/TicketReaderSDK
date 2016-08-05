#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "genericcommands.h"
#include "../../includes/protocolconf.h"
#include "RS232.h"

#define MAX_DATA_LEN 247

Answer* GenericNoDataCmd(char cmd) {
	char command[9];
	
	command[0] = WUP;
	command[1] = STX;
	command[2] = DEST;
	command[3] = cmd;
	command[4] = ETX;
	command[5] = 0x30;
	command[6] = 0x30;
	command[7] = EOT;
	command[8] = 0x00;
	
	SendData(command);
	#ifdef DEBUG
		printf("DBG: Sent command: 0x%x\n", cmd);
	#endif
	
	char* ackCode = ReadData();
	if (ackCode[0] != ACK)
	{
		#ifdef DEBUG
			printf("DBG: 0x%x received\n", ackCode[0]);
		#endif
		
		return 0;
	}
	else
	{
		#ifdef DEBUG
			printf("DBG: ACK received\n");
		#endif
	}
	
	Reply *reply = malloc(sizeof(*reply));
	Answer *answer = malloc(sizeof(*answer));
	
	answer->cmdData[0] = 0x00;
	answer->cmdData[1] = 0x00;
	answer->cmdData[2] = 0x00;
	answer->cmdData[3] = 0x00;
	
	PollForAnswer(cmd, reply, answer);
	
	#ifdef DEBUG
		printf("DBG: Answer to command 0x%x: 0x%x 0x%x 0x%x\n", answer->replies[0]->cmd, answer->replies[0]->code[0], answer->replies[0]->code[1], answer->replies[0]->code[2]);
	
		if(reply->actualDataLen > 0)
		{
			printf("DBG: Received data:");
			
			int i;
			for(i=0; i<reply->actualDataLen; i++)
			{
				printf(" 0x%x", reply->data[i]);
			}
			
			printf("\n");			
		}
	#endif
	
	return answer;
}

Answer* GenericCmd(char cmd, char* data, int dataLen) {
	char command[9 + dataLen];
	int i;
	
	command[0] = WUP;
	command[1] = STX;
	command[2] = DEST;
	command[3] = cmd;
	
	#ifdef DEBUG
		printf("DBG: Sending command data:");
	#endif
	
	for(i=0; i<dataLen; i++)
	{
		command[4 + i] = data[i];
		
		#ifdef DEBUG
			printf(" 0x%x", data[i]);
		#endif
	}
	
	#ifdef DEBUG
		printf("\n");
	#endif
	
	command[4 + dataLen] = ETX;
	command[5 + dataLen] = 0x30;
	command[6 + dataLen] = 0x30;
	command[7 + dataLen] = EOT;
	command[8 + dataLen] = 0x00;
	
	SendData(command);
	#ifdef DEBUG
		printf("DBG: Sent command: 0x%x\n", cmd);
	#endif
	
	char* ackCode = ReadData();
	if (ackCode[0] != ACK)
	{
		#ifdef DEBUG
			printf("DBG: 0x%x received\n", ackCode[0]);
		#endif
		
		return 0;
	}
	else
	{
		#ifdef DEBUG
			printf("DBG: ACK received\n");
		#endif
	}
	
	Reply *reply = malloc(sizeof(*reply));
	Answer *answer = malloc(sizeof(*answer));
	
	for(i=0; i<dataLen;i++)
	{
		answer->cmdData[i] = data[i];	
	}
	
	PollForAnswer(cmd, reply, answer);
	
	#ifdef DEBUG
		printf("DBG: Answer to command 0x%x: 0x%x 0x%x 0x%x\n", answer->replies[0]->cmd, answer->replies[0]->code[0], answer->replies[0]->code[1], answer->replies[0]->code[2]);
	
		if(reply->actualDataLen > 0)
		{
			printf("DBG: Received data:");
			
			for(i=0; i<reply->actualDataLen; i++)
			{
				printf(" 0x%x", reply->data[i]);
			}
			
			printf("\n");			
		}
	#endif
	
	return answer;
}

void PollForAnswer(char cmd, Reply* reply, Answer* answer) {
	char pollCommand[3];
	
	pollCommand[0] = WUP;
	pollCommand[1] = ENQ;
	pollCommand[2] = 0x00;
	
	reply->actualDataLen = 0;
	reply->cmd = cmd;
	reply->code[0] = 0x30;
	reply->code[1] = 0x31;
	reply->code[2] = 0x30;
	reply->data = malloc(MAX_DATA_LEN * sizeof(char));
				
	answer->replies[0] = reply;
	answer->repliesNum = 1;
	
	int pollRetries = 0;
	do
	{
		#ifdef DEBUG
			printf("DBG: Polling\n");
		#endif
		SendData(pollCommand);
		char* pollAnswer = ReadData();
		
		if(pollAnswer[0] != DLE)
		{
			if(pollAnswer[0] == STX && pollAnswer[1] == DEST && pollAnswer[2] == cmd)
			{
				reply->code[0] = pollAnswer[3];
				reply->code[1] = pollAnswer[4];
				reply->code[2] = pollAnswer[5];
				
				int i;
				for(i = 0; i < MAX_DATA_LEN; i++)
				{
					if(pollAnswer[6 + i] == ETX)
					{
						reply->data[i] = 0x00;
						break;
					}
					else
					{
						reply->data[i] = pollAnswer[6 + i];
						reply->actualDataLen++;
					}
				}
				
				return;
			}
		}
		else
		{
			millisleep(300);
		}
		
		pollRetries++;
	}
	while(pollRetries < 10);
	
	return;
}

void millisleep(int millisecs) {
	#ifdef DEBUG
		printf("DBG: Waiting %d ms...\n", millisecs);
	#endif
	struct timespec sleepTime;
	sleepTime.tv_sec = 0;
	sleepTime.tv_nsec = millisecs * 1000000L;
	nanosleep(&sleepTime, NULL);
}
