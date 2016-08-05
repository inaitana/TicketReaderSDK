#ifndef COMMANDS_H_
#define COMMANDS_H_

void PrintMessage(char* message);
void PrintError(char* errorMessage);

int SetParameter(char paramID[2], char paramValue[4]);
int* GetSensorStatus();
int SetLedStatus(int red, int green);

int InsertTicket();
int InsertTicketSteps(char steps[4]);
int EjectTicket();
int CaptureTicket();
char* ReadTicket();

#endif
