#ifndef GENERICCOMMANDS_H_
#define GENERICCOMMANDS_H_

typedef struct {
	int actualDataLen;
	char cmd;
	char code[3];
	char *data;
} Reply;

typedef struct {
	char cmdData[6];
	Reply* replies[4];
	int repliesNum;
} Answer;

Answer* GenericNoDataCmd(char cmd);
Answer* GenericCmd(char cmd, char* data, int dataLen);
void PollForAnswer(char cmd, Reply* reply, Answer* answer);
void millisleep(int millisecs);

#endif
