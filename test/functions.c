#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../src/sdk/genericcommands.h"
#include "../src/commands.h"
#include "functions.h"

#define WAITING_SLEEP_TIME 10
#define BLINKING_SLEEP_TIME 300

int waitForStatus(int statusIndex, int statusValue, long timeout)
{
	int* status;	
	
	struct timeval startTime;
	struct timeval testTime;
	long timeDiff;
	
	if (timeout > 0)
	{
		gettimeofday(&startTime, NULL);
	}

	do {
		status = GetSensorStatus();
		
		if(status[statusIndex] == statusValue)
		{
			return 1;
		}
		else
		{
			millisleep(WAITING_SLEEP_TIME);
		}
		
		free(status);
		
		if (timeout > 0)
		{
			gettimeofday(&testTime, NULL);
			timeDiff = (testTime.tv_usec + 1000000 * testTime.tv_sec) - (startTime.tv_usec + 1000000 * startTime.tv_sec);			
		}
	}
	while(timeout == 0 || timeDiff < (timeout * 1000));
	
	return 0;
}

int waitForStatusAndBlink(int statusIndex, int statusValue, int blinkGreen, int blinkRed, long timeout)
{
	int* status;
	
	int yesNo = 0;
	
	struct timeval startTime;
	struct timeval testTime;
	long timeDiff;
			
	if (timeout > 0)
	{
		gettimeofday(&startTime, NULL);
	}
	
	do {
		status = GetSensorStatus();
		
		if(status[statusIndex] == statusValue)
		{
			SetLedStatus(0, 0);
			return 1;
		}
		else
		{
			if(yesNo == 0)
			{
				if(blinkGreen)
				{
					SetLedStatus(1, 0);
				}
				else if (blinkRed)
				{
					SetLedStatus(0, 1);
				}		
			}
			else
			{
				SetLedStatus(0, 0);
			}
			yesNo = !yesNo;
			
			millisleep(BLINKING_SLEEP_TIME);
		}
		
		free(status);
		
		if (timeout > 0)
		{
			gettimeofday(&testTime, NULL);
			timeDiff = (testTime.tv_usec + 1000000 * testTime.tv_sec) - (startTime.tv_usec + 1000000 * startTime.tv_sec);			
		}
	}
	while(timeout == 0 || timeDiff < (timeout * 1000));
	
	return 0;
}
