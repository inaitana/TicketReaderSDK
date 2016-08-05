#ifndef RS232_H_
#define RS232_H_

int OpenSerialComm();
void CloseSerialComm();

char* ReadData();
void SendData(char *msg);

#endif
