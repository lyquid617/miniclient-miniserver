#ifndef __MINISERVER_USING_TCP__
#define __MINISERVER_USING_TCP__
//used in Linux or Mac
#include "minitcp.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


// packet :has type and data
struct packet
{
    typeOfPacket packetType;
    int type;
    char data[MAXDATALEN];
};

//ID of thread tid,store in link
struct TIDLIST
{
    pthread_t tid;
    struct TIDLIST *next;
};
struct TIDLIST *Start_node;
struct TIDLIST *Current_node;

//array to store sockfd ,
//if no one connect , be zero
struct fd_client
{
    int tail;
    // fd: file descriptor
    int fd[LISTENQUEUESIZE];
};
struct fd_client cfd;//cfd: client of file descriptor

struct CLIENTLIST
{
    int fd;
    unsigned short port;
    struct in_addr addr;// not the internet address
};
struct CLIENTLIST list_client[LISTENQUEUESIZE];

int fd_of_server;

//posix下抽象了一个锁类型的结构：ptread_mutex_t。通过对该结构的操作，来判断资源是否可以访问
//PTHREAD_MUTEX_INITIALIZER则是一个结构常量。
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//handle the exit
static void ExitHandler(int signal);

//init the TID list
int initTidList();

/*
    if accept success will create a pthread to handle the connect
    if accept failed  will kill this function
*/
int IsAcceptConnect();

//handle the new connect
//虽然这里的s没有用到,但是为了符合 void* (*)(void*)的格式.必须有这个
void *handle_Connect(void *s);

//create the new connect pthread
int create_connection_pthread();


//analysis the packet;generate response
int handle_Packet(struct packet *one_packet, int fd);

//use this function to response  time
void handle_Time_Packet(struct packet *one_packet, int fd);
//use this function to response disconnect
int handle_Disconnect_Packet(struct packet *one_packet, int fd);
//use this function to response name
void handle_Name_Packet(struct packet *one_packet, int fd);
//use this function to response message
void handle_Message_Packet(struct packet *one_packet, int fd);
//use this function to response list
void handle_List_Packet(struct packet *one_packet, int fd);



#endif // !__MINISERVER_USING_TCP__