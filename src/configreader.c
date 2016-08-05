#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "configreader.h"

char* getConfigSetting(char *filename, char *settingName)
{
	FILE *fp;
	int result;
	char *readSettingName = (char*) malloc(255 * sizeof(char));
	char *readSettingValue = (char*) malloc(255 * sizeof(char));
	
	if(access(filename, R_OK) == -1) {
		printf("Missing settings file\n");
		return 0;
	}
	
	fp = fopen(filename, "r");
	
	do
	{
		result = fscanf(fp, " %s = %s ", readSettingName, readSettingValue);
		if(strcmp(readSettingName, settingName) == 0)
		{
			fclose(fp);
			return readSettingValue;
		}
	}
	while(result != EOF);

	fclose(fp);
	printf("Setting not found in settings file\n");
	return 0;
}
