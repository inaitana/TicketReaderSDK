#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include "RS232.h"

#define BAUDRATE B9600
#define _POSIX_SOURCE 1 // POSIX compliant source

#define MAX_READ_LENGTH 255

int conn; // the RS232 connection
struct termios oldtio, newtio; // old port settings, new port settings
char buf[MAX_READ_LENGTH + 1]; // buffer where incoming data will be read

int OpenSerialComm(char* port) {
	conn = open(port, O_RDWR | O_NOCTTY); // open connection in read/write, don't take control of the serial port
	if (conn < 0) {perror(port); return -1; } // if connection fails, print the error and return -1
	
	tcgetattr(conn, &oldtio); // backup current port settings
	
	memset(&newtio, '\0', sizeof(newtio)); // clear struct for new port settings
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD; // BAUD 9600, 8 no parity, don't change owner, can read
 	newtio.c_iflag = IGNPAR; // ignore parity
	newtio.c_oflag = ~(ICANON | ECHO | ECHOE | ISIG); // non-canonical input, don't echo characters, don't echo deletes, don't accept signals
	newtio.c_cc[VMIN] = MAX_READ_LENGTH; // read max 255 characters before returning
	newtio.c_cc[VTIME] = 1; // wait max 0.1 seconds before each read character before closing the read 
	

	tcflush(conn, TCIFLUSH); // flush the port
	tcsetattr(conn, TCSANOW, &newtio); // set new port settings

	return 0;
}

void CloseSerialComm() {
	tcsetattr(conn, TCSANOW, &oldtio); // restore old port settings
	close(conn); // close the connection
}

char* ReadData() {
	int n;
	
	memset(buf, 0, sizeof(buf)); // empty the buffer
	
	n = read(conn, buf, sizeof(buf)); // read data from the connection to the buffer
	buf[n]=0x00; // terminate read data with a \0 for extra security
		
	/* if it was compiled in debug mode, print read data */
	#ifdef DEBUG
		int i;
		
		printf("DBG: RX: ");
		for(i = 0; i<strlen(buf); i++) {
			printf("%02X ", buf[i]);
		}
		printf("\n");
	#endif
	
	return buf;
}

void SendData(char *msg) {
	write(conn, msg, strlen(msg)); // write data from msg to the connection
	
	/* if it was compiled in debug mode, print sent data */
	#ifdef DEBUG
		int i;
		
		printf("DBG: TX: ");
		for(i = 0; i<strlen(msg); i++) {
			printf("%02X ", msg[i]);
		}
		printf("\n");
	#endif
}
