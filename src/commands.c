#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../src/sdk/RS232.h"
#include "../src/sdk/genericcommands.h"
#include "../includes/protocolconf.h"
#include "commands.h"

#define DEFAULT_INSERT_STEPS "0100"

void PrintMessage(char* message)
{
	time_t now;
	struct tm* nowstruct;
	char nowstring[20];
	
	now = time(0);
	nowstruct = localtime(&now);
	strftime(nowstring, 20, "%d/%m/%Y %H:%M:%S", nowstruct);
	
	fprintf(stdout, "%s - %s\n", nowstring, message);
	fflush(stdout);
}

void PrintError(char* errorMessage)
{
	time_t now;
	struct tm* nowstruct;
	char nowstring[20];
	
	now = time(0);
	nowstruct = localtime(&now);
	strftime(nowstring, 20, "%d/%m/%Y %H:%M:%S", nowstruct);
	
	fprintf(stderr, "%s - ERROR: %s\n", nowstring, errorMessage);
	fflush(stderr);
}

int SetParameter(char paramID[2], char paramValue[4])
{	
	#ifdef DEBUG
		printf("DBG: Setting parameter '%s' to value '%s'\n", paramID, paramValue);
	#endif
	
	int result;
				
	Answer* answer;
	
	char payload[6];
	payload[0] = paramID[0];
	payload[1] = paramID[1];
	payload[2] = paramValue[0];
	payload[3] = paramValue[1];
	payload[4] = paramValue[2];
	payload[5] = paramValue[3];
	
	answer = GenericCmd(CMD_SET, payload, 6);
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		#ifndef SILENTSET
			PrintError("Generic error during set parameter");
		#endif
		result = 0;
	}
	else if(answer->replies[0]->code[1] != 0x30)
	{
		#ifndef SILENTSET
			PrintError("Generic error during set parameter");
		#endif
		result = 0;
	}
	else if(answer->replies[0]->code[2] != 0x30)
	{
		#ifndef SILENTSET
			PrintError("Generic error during set parameter");
		#endif
		result = 0;
	}
	else
	{
		result = 1;
	}
	
	free(answer->replies[0]);
	free(answer);
	
	return result;
}

int* GetSensorStatus()
{
	#ifdef DEBUG
		printf("DBG: Getting sensor status\n");
	#endif
	
	int* result;
	
	Answer* answer;
	int* status = malloc(4 * sizeof(int));
	
	answer = GenericNoDataCmd(CMD_STATUS);	
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		PrintError("Generic error during get sensor status");
		result = 0;
	}
	else if(answer->replies[0]->code[1] != 0x30)
	{
		PrintError("Generic error during get sensor status");
		result = 0;
	}
	else if(answer->replies[0]->code[2] != 0x30)
	{
		PrintError("Generic error during get sensor status");
		result = 0;
	}
	else
	{
		status[0] = (answer->replies[0]->data[0] == '0') ? 0 : 1;
		status[1] = (answer->replies[0]->data[1] == '0') ? 0 : 1;
		status[2] = (answer->replies[0]->data[2] == '0') ? 0 : 1;
		status[3] = (answer->replies[0]->data[3] == '0') ? 0 : 1;
		
		result = status;
	}
	
	free(answer->replies[0]->data);
	free(answer->replies[0]);
	free(answer);
	
	return result;
}

int SetLedStatus(int green, int red)
{
	#ifdef DEBUG
		printf("DBG: Setting leds status: green '%d', red '%d'\n", green, red);
	#endif
	
	int result;
	
	Answer* answer;
	char ledStatus[4];
	memset(&ledStatus, '0', 4);
	
	if(green)
	{
		ledStatus[0] = 'F';
		ledStatus[1] = 'F';
	}
	if(red)
	{
		ledStatus[2] = 'F';
		ledStatus[3] = 'F';
	}
	
	answer = GenericCmd(CMD_LEDS, ledStatus, 4);
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		PrintError("Generic error during set led status");
		result = 0;
	}	
	else if(answer->replies[0]->code[1] != 0x30)
	{
		PrintError("Generic error during set led status");
		result = 0;
	}	
	else if(answer->replies[0]->code[2] != 0x30)
	{
		PrintError("Generic error during set led status");
		result = 0;
	}
	else
	{
		result = 1;
	}
	
	free(answer->replies[0]);
	free(answer);
	
	return result;
	
}

int InsertTicket()
{
	return InsertTicketSteps(DEFAULT_INSERT_STEPS);
}

int InsertTicketSteps(char steps[4])
{
	#ifdef DEBUG
		printf("DBG: Inserting ticket with steps offset '%s'\n", steps);
	#endif
	
	int result;
				
	Answer* answer;
	
	answer = GenericCmd(CMD_INSERT, steps, 4);
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		PrintError("Generic error during insert ticket");
		result = 0;
	}
	else if(answer->replies[0]->code[2] != 0x30)
	{
		PrintError("Generic error during insert ticket");
		result = 0;
	}
	else
	{	
		switch (answer->replies[0]->code[1])
		{
			case 0x30:
				result = 1;
				break;
			case 0x31:
				PrintError("Jamming error");
				result = 0;
				break;
			case 0x32:
				PrintError("Cannot insert ticket");
				result = 0;
				break;
			case 0x38:
				PrintError("No ticket in mouth");
				result = 0;
				break;
			default:
				PrintError("Generic error during insert ticket");
				result = 0;
				break;
		}
	}	
	
	free(answer->replies[0]);
	free(answer);
	
	return result;
}

int EjectTicket()
{
	#ifdef DEBUG
		printf("DBG: Ejecting ticket\n");
	#endif
	
	int result;
				
	Answer* answer;
	
	answer = GenericNoDataCmd(CMD_EJECT);
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		PrintError("Generic error during eject ticket");
		result = 0;
	}
	else if(answer->replies[0]->code[2] != 0x38)
	{
		PrintError("Generic error during eject ticket");
		result = 0;
	}
	else
	{	
		switch (answer->replies[0]->code[1])
		{
			case 0x30:
				result = 1;
				break;
			case 0x31:
				PrintError("No ticket inside reader");
				result = 0;
				break;
			case 0x32:
				PrintError("Cannot eject ticket");
				result = 0;
				break;
			case 0x34:
				PrintError("No ticket inside reader");
				result = 0;
				break;
			default:
				PrintError("Generic error during eject ticket");
				result = 0;
				break;
		}
	}
	
	free(answer->replies[0]);
	free(answer);
	
	return result;
}

int CaptureTicket()
{
	#ifdef DEBUG
		printf("DBG: Capturing ticket\n");
	#endif
	
	int result;
				
	Answer* answer;
	
	answer = GenericNoDataCmd(CMD_CAPTURE);
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		PrintError("Generic error during capture ticket");
		result = 0;
	}
	else if(answer->replies[0]->code[2] != 0x30)
	{
		PrintError("Generic error during capture ticket");
		result = 0;
	}
	else
	{
		switch (answer->replies[0]->code[1])
		{
			case 0x30:
				result = 1;
				break;
			case 0x31:
				PrintError("Transport failure");
				result = 0;
				break;
			case 0x34:
				PrintError("No ticket inside reader");
				result = 0;
				break;
			default:
				PrintError("Generic error during capture ticket");
				result = 0;
				break;
		}		
	}	
	
	free(answer->replies[0]);
	free(answer);
	
	return result;
}

char* ReadTicket()
{
	#ifdef DEBUG
		printf("DBG: Reading ticket\n");
	#endif
	
	char* result;
				
	Answer* answer;
	
	answer = GenericNoDataCmd(CMD_READ);
	
	if(answer->replies[0]->code[0] != 0x30)
	{
		PrintError("Generic error during read ticket");
		result = 0;
	}
	else if(answer->replies[0]->code[2] != 0x30)
	{
		PrintError("Generic error during read ticket");
		result = 0;
	}
	else
	{
		switch (answer->replies[0]->code[1])
		{
			case 0x30:
				result = answer->replies[0]->data;
				break;
			case 0x34:
				PrintError("Can't read barcode");
				result = 0;
				break;
			default:
				PrintError("Generic error during read ticket");
				result = 0;
				break;
		}		
	}	
	
	free(answer->replies[0]);
	free(answer);
	
	return result;
}
