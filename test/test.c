#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/sdk/RS232.h"
#include "../includes/protocolconf.h"
#include "../src/configreader.h"
#include "../src/commands.h"
#include "functions.h"


#define CONFIGFILE "../conf/test.conf"
#define DEFAULT_PORT "/dev/ttyUSB0"

#define DEFAULT_TIMEOUT 20000
#define MAX_RETRIES 2

int main(int argc, char *argv[])
{	
	char* port = getConfigSetting(CONFIGFILE, "port");
	if(port == 0)
	{
		port = DEFAULT_PORT;
	}
	
	if(OpenSerialComm(port) == -1)
	{
		exit(-1);
	}
	
	char choice, ledChoice;
	int quit = 0;
	
	int* status;
	char* barcode;
	
	int waitSuccess;
	
	SetParameter(PARAM_EJECTSTEPS, "024E");
		
	while(quit == 0)
	{
		if(argc == 2 && strcmp(argv[1], "auto") == 0)
		{
			choice = 'a';
		}
		else
		{
			printf("\ncommand? [s]tatus [i]nsert [e]ject [c]apture [r]ead [l]eds [a]utoscan [q]uit\n");
			choice = getchar();
			getchar();
		}
		
		#ifdef DEBUG
			printf("DBG: Choice is '%c'\n", choice);
		#endif
		
		switch(choice) {
			case 's':
				status = GetSensorStatus();
				
				if(status[0] == 0)
				{
					printf("Ticket available in mouth\n");
				}
				else
				{
					printf("No ticket in mouth\n");
				}
				
				if(status[1] == 0)
				{
					printf("Ticket inside reader\n");
				}
				else
				{
					printf("No ticket inside reader\n");
				}
				
				free(status);
				break;
			case 'i':
				InsertTicket();
				break;
			case 'e':
				EjectTicket();
				break;
			case 'c':
				CaptureTicket();
				break;
			case 'r':
				status = GetSensorStatus();
				if(status[1] == 1)
				{
					if(status[0] == 0)
					{
						InsertTicket();
					}
					else
					{
						printf("No ticket to read\n");
						break;
					}	
				}
				
				free(status);
				
				barcode = ReadTicket();
				if(barcode != 0)
				{
					printf("Read barcode: %s\n", barcode);
				}
				else
				{
					printf("No barcode read\n");
				}
				
				free(barcode);
				
				EjectTicket();
				
				break;
			case 'l':
				printf("new led status? [o]ff [g]reen [r]ed [b]oth\n");
				ledChoice = getchar();
				getchar();
		
				#ifdef DEBUG
					printf("DBG: LED choice is '%c'\n", choice);
				#endif
		
				switch(ledChoice)
				{
					case 'o':
						SetLedStatus(0, 0);
						break;
					case 'g':
						SetLedStatus(1, 0);
						break;
					case 'r':
						SetLedStatus(0, 1);
						break;
					case 'b':
						SetLedStatus(1, 1);
						break;					
				}
				break;
			case 'a':
				status = GetSensorStatus();
				{
					if(status[1] == 0)
					{
						EjectTicket();
					}
				}
				
				free(status);
				
				waitForStatusAndBlink(0, 0, 1, 0, 60000);
				
				while(1)
				{
					waitForStatus(0, 0, 0);
					
					int retries = 0;
					do {						
						if (InsertTicket())
						{							
							barcode = ReadTicket();
						}
						else
						{
							barcode = 0;
						}
						
						if(barcode != 0)
						{
							SetLedStatus(1, 0);
							retries = MAX_RETRIES;
						}
						else
						{
							SetLedStatus(0, 1);
						}
						
						EjectTicket();
						
						retries++;
					}
					while(retries < MAX_RETRIES);
					
					if(barcode == 0)
					{
						waitSuccess = waitForStatusAndBlink(0, 1, 0, 1, DEFAULT_TIMEOUT);
					}
					else
					{
						waitSuccess = waitForStatus(0, 1, DEFAULT_TIMEOUT);
					}
					
					if(waitSuccess == 0)
					{
						CaptureTicket();
					}					
					
					SetLedStatus(0, 0);
						
					free(barcode);
				}
				break;
			case 'q':
				quit = 1;
				break;
		}
	}
	
	CloseSerialComm();
	
	return 0;
}
