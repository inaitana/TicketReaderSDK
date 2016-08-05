#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

int waitForStatus(int statusIndex, int statusValue, long timeout);
int waitForStatusAndBlink(int statusIndex, int statusValue, int blinkGreen, int blinkRed, long timeout);

#endif
