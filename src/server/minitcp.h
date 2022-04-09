#ifndef __MINITCP__
#define __MINITCP__

#ifndef TRUE
    #define TRUE 1
#endif

//listen queue size
#define LISTENQUEUESIZE 12
//size of packet buffer
#define BUFFERSIZE 300
//the port of server 4548
#define PORTSERVER 4548
// max size of date
#define MAXDATALEN 256

typedef enum
{
    REQUEST = 1,
    RESPONSE,
    INSTRUCT
} typeOfPacket;
typedef enum
{
    TIME = 1,
    NAME,
    LIST,
    MESSAGE,
    DISCONNECT
} typeOfRequest;
typedef enum
{
    CORRECT = 1,
    WRONG
} typeOfResponse;
typedef enum
{
    FORWARD = 1,
    TERMINATE
} typeOfInstruct;



#endif // !__MINITCP__