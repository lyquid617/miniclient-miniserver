#ifndef __MINICLIENT__
#define __MINICLIENT__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/shm.h>

#define bool char
#define true 1
#define false 0

#define MAXDATALEN 256
typedef enum { REQUEST = 1, RESPONSE, INSTRUCT } packetType;
typedef enum { TIME = 1, NAME, LIST, MESSAGE ,DISCONNECT} requestType;
typedef enum { FORWARD = 1, TERMINATE } instructType;

typedef struct spacket {
	packetType pType;
	int type;
	char data[MAXDATALEN]; 
} packet;

int socketfd;
pthread_t p;

bool connect2Server(const char *ip,const char *port);
void sendDisRequestPacket(void);
void sendTimeRequestPacket(void);
void sendNameRequestPacket(void);
void sendListRequestPacket(void);
void sendMessageRequestPacket(const char *client,const char *msg);


#endif