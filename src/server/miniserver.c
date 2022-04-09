/*******************************************
 * used in Linux or Mac
 * written by JiMa 
 * used as a mini server using TCP 
 * file type : UTF-8 
 * ****************************************/
#include "miniserver.h"
/*
about sockaddr_in
struct sockaddr_in{
    short int sin_family;
    unsigned short int sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];//应该使用函数bzero()或者memset()来置零,sin_port和sin_addr必须是网络字节顺序
}
sin_zero表示填充字节，一般情况下该值为0；
sin_family表示地址类型，对于基于TCP/IP传输协议的通信，该值只能是AF_INET
inet_addr可以讲IP地址从点数格式转换成无符号长整形
inet_addr()返回的地址已经是网络字节格式，所以你无需再调用 函数htonl()

*/

/* about select 
select() can implement the unblock programming
int select(int nfds,  fd_set* readset,  fd_set* writeset,  fe_set* exceptset,  struct timeval* timeout);
       nfds           需要检查的文件描述字个数
       readset     用来检查可读性的一组文件描述字。
       writeset     用来检查可写性的一组文件描述字。
       exceptset  用来检查是否有异常条件出现的文件描述字。(注：错误不包括在异常条件之内)
       timeout      超时，填NULL为阻塞，填0为非阻塞，其他为一段超时时间
return value: 返回fd的总数，错误时返回SOCKET_ERROR

fd_set其实这是一个数组的宏定义，实际上是一long类型的数组，
每一个数组元素都能与一打开的文件句柄(socket、文件、管道、设备等)建立联系，建立联系的工作由程序员完成，
当调用select()时，由内核根据IO状态修改fd_set的内容，由此来通知执行了select()的进程哪个句柄可读

*/



int main(int argc, char *argv[])
{
    int sizeof_sockaddr_in = 0;
    struct sockaddr_in server_addr;

    //在POSIX兼容平台上，SIGINT是当用户希望中断进程时由其控制终端发送给进程的信号。当进程控制终端上的用户按下正在运行的进程密钥 - 通常是Control-C，
    signal(SIGINT, ExitHandler);
    printf("Initialize the Server !\nserver IP:127.0.0.1\n");

    /**************--initialize the socket---******************/
    //init the array of client socketfd
    cfd.tail = 0;
    memset(cfd.fd, LISTENQUEUESIZE, 0);
    for (int i = 0; i < LISTENQUEUESIZE; ++i)list_client[i].fd = 0;

    server_addr.sin_family = AF_INET; //TCP
    /*
        将一个无符号短整型数值转换为网络字节序，即大端模式(big-endian)　　参数u_short hostshort: 16位无符号整数　返回值:
               TCP / IP网络字节顺序.
    */
    server_addr.sin_port = htons(PORTSERVER);
    /*
        将一个32位数从主机字节顺序转换成网络字节顺序。　　
    返回值：htonl()返回一个网络字节顺序的值
    */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bzero()将字符串s的前n个字节置为0，一般来说n通常取sizeof(s),将整块空间清零
    bzero(&(server_addr.sin_addr), 8);
    sizeof_sockaddr_in = sizeof(struct sockaddr_in);

    //new server fd
    /*
        SOCK_STREAM是基于TCP的，数据传输比较有保障。SOCK_DGRAM是基于UDP的，专门用于局域网，
    */
    fd_of_server = socket(AF_INET, SOCK_STREAM, 0);

    /**************--bind the server socket---******************/
    /*
        bind() :
        将address指向的sockaddr结构体中描述的一些属性（IP地址、端口号、地址簇）与socket套接字绑定，也叫给套接字命名。
        调用bind()的时候,可以使用1024~65535的任意一个端口,如果使用connect(),会检测socket有没有绑定端口,如果没有,会自己绑定一个没有使用过的本地端口
    */
    if (bind(fd_of_server, (struct sockaddr *)&server_addr, sizeof_sockaddr_in) < 0)
    {
        printf("ERR! BIND FAILED\n");
        return 0;
    }
    printf("BIND START!\n");
    //listen the client connect
    /*
    about listen()
    int listen(int sockfd, int backlog); sockfd 是调用socket() 返回的套接字文件描述符。backlog 是在进入队列中允许的连接数目。
    */
    if (listen(fd_of_server, LISTENQUEUESIZE) < 0)
    {
        printf("ERR! LISTEN FAILED\n");
        return 0;
    }

    printf("LISTEN START!SERVER START!\n");
    while (TRUE)
    {   
        // dead loop and continue listenning
        IsAcceptConnect();
    }

    return 0;
}

static void ExitHandler(int signal)
{
    //加锁解锁,对于一个资源有互斥存在时Lock,操作完unlock
    pthread_mutex_lock(&mutex);
    printf("\n");
    for (int i = 0; i < LISTENQUEUESIZE; ++i)
    {
        if (list_client[i].fd > 0)
        {
            // send_Exit_Packet
            struct packet exit_packet;
            exit_packet.packetType = INSTRUCT;
            exit_packet.type = TERMINATE;
            memset(exit_packet.data,0,MAXDATALEN);
            sprintf(exit_packet.data, "\n(MiniServer) closed server\n");
            //Send N bytes of BUF to socket FD.  Returns the number sent or -1.
            send(list_client[i].fd, (char *)&exit_packet, sizeof(exit_packet), 0);
            printf("packetType:%d type:%d Socketfd:%d connect close\n", exit_packet.packetType, exit_packet.type, list_client[i].fd);

            printf("Socketfd:%d connect close \n", list_client[i].fd);
            //close the fd
            close(list_client[i].fd);
            list_client[i].fd = 0;
        }
    }
    pthread_mutex_unlock(&mutex);
    close(fd_of_server);
    printf("\nGoodBye! Server Exit!\n");
    exit(0);
}

/* 
    if init tid list success return 0;otherwise return -1
*/
int initTidList()
{
    Start_node = (struct TIDLIST *)malloc(sizeof(struct TIDLIST));
    if (Start_node == NULL)
    {
        printf("ERR! Init tid list FAILED!");
        return -1;
    }
    Start_node->next = NULL;
    Current_node = Start_node;//move the pointer `current_node` to start
    return 0;
}


//Await a connection on socket FD
int IsAcceptConnect()
{
    int temp_fd = 0;//temp fd
    unsigned int sizeof_sockaddr_in = 0;
    
    struct sockaddr_in addr_of_client;
    //whether cfd is full
    int IsCFD_full = 0;

    sizeof_sockaddr_in = sizeof(struct sockaddr_in);
    //do not beyond the LISTENQUEUESIZE
    if (cfd.tail < LISTENQUEUESIZE)
    {
        int tmp = accept(fd_of_server, (struct sockaddr *)&addr_of_client, &sizeof_sockaddr_in);
        // source mutex
        pthread_mutex_lock(&mutex);
        // normal linklist operation:add node
        cfd.fd[cfd.tail] = tmp;
        cfd.tail++;
        IsCFD_full = TRUE;
        for (int i = 0; i < LISTENQUEUESIZE; ++i)
        {
            if (list_client[i].fd == 0)
            {
                list_client[i].fd = cfd.fd[cfd.tail - 1];
                list_client[i].port = addr_of_client.sin_port;
                list_client[i].addr = addr_of_client.sin_addr;
                //inet_ntoa()用来将参数in 所指的网络二进制的数字转换成网络地址, 然后将指向此网络地址字符串的指针返回.
                printf(" Socketfd:%d Port:%hu IP:%s Connecting\n",cfd.fd[cfd.tail - 1], list_client[i].port,inet_ntoa(list_client[i].addr));
                IsCFD_full = 0;
                break;
            }
        }
        if (IsCFD_full!=TRUE)
        {
            printf("socketfd:%d connect SUCCESSFULLY!\n", cfd.fd[cfd.tail-1]);
            create_connection_pthread();
                    }
        else
        {
            printf("Socketfd:%d connect FAILED!\n", cfd.fd[cfd.tail-1]);

        }
        pthread_mutex_unlock(&mutex);
    }
    else
    {
        printf("Client Connect is full!\n");
    }
    return 0;
}

/*
    for each socket connect
*/
int create_connection_pthread()
{
    pthread_attr_t thread_attr;
    pthread_t tid;
    /*
    Initialize thread attribute *ATTR with default attributes
    */
    pthread_attr_init(&thread_attr);
#ifdef __DEBUG__
    printf("attribute init succeed!\n");
#endif
    pthread_create(&tid, &thread_attr, handle_Connect, (void *)NULL);
#ifdef __DEBUG__
    printf("create pthread succeed!\n");
#endif
    return 0;
}

/*
    handle each socket connect
*/
void *handle_Connect(void * s)
{
    int fd, num_of_bytes;
    struct packet pckt;
    memset(pckt.data,0,MAXDATALEN);
    printf("Start get connect Socketfd\n");
    pthread_mutex_lock(&mutex);
    cfd.tail--;
    fd = cfd.fd[cfd.tail];
    pthread_mutex_unlock(&mutex);
    //after connect succeed send a packet to ensure
    printf("get connect Socketfd:%d\n", fd);


    // send_Hello:after socket connect successfully,the server send a packet
    struct packet hello_packet;
    memset(hello_packet.data,0,MAXDATALEN);
    hello_packet.packetType = RESPONSE;
    hello_packet.type = CORRECT;
    printf("start generate hello packet data\n");
    sprintf(hello_packet.data, "\n(MiniServer) Socketfd:%d connect succeed!\n", fd);
    printf("generate hello packet data succeed\n");
    //Send N bytes of BUF to socket FD.  Returns the number sent or -1.
    send(fd, (char *)&hello_packet, sizeof(hello_packet), 0);


    while (TRUE)
    {   
        //recv return the value it read or retunr -1 if error occur
        num_of_bytes = recv(fd, (char *)&pckt, sizeof(pckt), 0);
        if (num_of_bytes < 0)
        {
            printf("from socketfd:%d get error packet\n", fd);
            break;
        }
        printf("from socketfd:%d get packet\n", fd);
        printf("from socketfd:%d packet packetType:%d, type:%d,data:%s\n", fd, pckt.packetType, pckt.type, pckt.data);
        if (handle_Packet(&pckt, fd) == -1)
        {
            return NULL;
        }
    }
    //必须有这一句,否则会在g++编译的时候报错:warning: control reaches end of non-void function
    return NULL;
}

/*
    to analyse the packet and make response
*/
int handle_Packet(struct packet *one_packet, int fd)
{   
    switch(one_packet->packetType){
        case REQUEST:{
            switch (one_packet->type)
            {
                case TIME:
                    handle_Time_Packet(one_packet, fd);
                    break;
                case NAME:
                    handle_Name_Packet(one_packet, fd);
                    break;
                case LIST:
                    handle_List_Packet(one_packet, fd);
                    break;
                case MESSAGE:
                    handle_Message_Packet(one_packet, fd);
                    break;
                case DISCONNECT:
                    handle_Disconnect_Packet(one_packet, fd);
                    break;
                
                default:
                    printf("Error type!\n");
                    break;
            }
        }
        case RESPONSE:{
            printf("Shouldn't receive RESPONSE packet!\n");
            break;
        }
        case INSTRUCT:{
            printf("Shouldn't receive INSTRUCT packet!\n");
            break;
        }
        default:{
            printf("ERR! Error type packet!\n");
            break;
        }
    };
    return 0;
}

/*
    response packet for time request
*/
void handle_Time_Packet(struct packet *one_packet, int fd)
{
    struct packet socket_packet;
    //其实就是一个long
    time_t ttime;
    struct tm *local_time;
    memset(socket_packet.data,0,MAXDATALEN);
    //get the current time
    time(&ttime);
    //get the time struuct representation
    local_time = localtime(&ttime);
    //using time format: pay attention to that year+=1900,month+=1
    int num_of_bytes = 
    sprintf(socket_packet.data,"(MiniServer) Data:%d:%d:%d Time:%d:%d:%d\n", 
    local_time->tm_year + 1900, local_time->tm_mon + 1,
    local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec);

    socket_packet.packetType = RESPONSE;
    socket_packet.type = CORRECT;
    send(fd, (char *)&socket_packet, sizeof(socket_packet), 0);
}
/*
    update the list_client for disconnect request
*/
int handle_Disconnect_Packet(struct packet *one_packet, int fd)
{
    printf("Disconnect Socketfd:%d\n", fd);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < LISTENQUEUESIZE; ++i)
    {   
        //find and reset fd (remove)
        if (fd == list_client[i].fd)
        {
            list_client[i].fd = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    close(fd);
    return -1;
}
/*
    response packet for name request
*/
void handle_Name_Packet(struct packet *one_packet, int fd)
{
    struct packet socket_packet;
    char host_name[128];
    memset(socket_packet.data,0,MAXDATALEN);
    //Put the name of the current host in no more than LEN bytes of NAME
    int hostres = gethostname(host_name, sizeof(host_name));
    if(hostres == -1)
    {
        printf("ERR! GET HOSTNAME FAILED!\n");
    }
    //write host into socketpactek data
    sprintf(socket_packet.data, "(MiniServer) %s\n", host_name);
    socket_packet.packetType = RESPONSE;
    socket_packet.type = CORRECT;
    send(fd, (char *)&socket_packet, sizeof(socket_packet), 0);
}



/*
    response packet fro MESSAGE request
*/
void handle_Message_Packet(struct packet *one_packet, int fd)
{
    struct packet socket_packet;
    memset(socket_packet.data,0,MAXDATALEN);
    // fd of destination
    int fd_of_destination = *((int *)one_packet->data);

    int IsExit = 0;
    printf("src_fd:%d des_fd:%d\n", fd, fd_of_destination);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < LISTENQUEUESIZE; ++i)
    {   
        //find and label the isExit as 1
        if (list_client[i].fd == fd_of_destination)
        {
            IsExit = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    if (IsExit == 1)
    {
        printf("Socketfd:%d Sending message to Socketfd:%d IsExit!\n", fd, fd_of_destination);
        printf("Message in packet:%s\n", one_packet->data + sizeof(int));
        socket_packet.packetType = INSTRUCT;
        socket_packet.type = FORWARD;
        //write into socketpactek data
        int thisI=0;
        for(int i=0;i<LISTENQUEUESIZE;i++){
            if(list_client[i].fd==fd){
                thisI=i;
            }
        }
        sprintf(socket_packet.data, "(MiniServer)src_fd:%d;src_ip:%s;src_port:%hu;%s\n", fd,inet_ntoa(list_client[thisI].addr),list_client[thisI].port,one_packet->data + sizeof(int));
        send(fd_of_destination, (char *)&socket_packet, sizeof(socket_packet), 0);
        struct packet response_packet;
        memset(response_packet.data,0,MAXDATALEN);
        response_packet.packetType = RESPONSE;
        response_packet.type = CORRECT;
        //write into response socketpacket data
        sprintf(response_packet.data, "(MiniServer) Message to Socketfd:%d send successfully!\n", fd_of_destination);
        send(fd, (char *)&response_packet, sizeof(response_packet), 0);
    }
    else
    {
        socket_packet.packetType = RESPONSE;
        socket_packet.type = CORRECT;
        int num_of_bytes = sprintf(socket_packet.data, "(MiniServer) No Socketfd:%d\b", fd_of_destination);
        send(fd, (char *)&socket_packet, sizeof(socket_packet), 0);
    }
}

/*
    response struct packet for List request
*/
void handle_List_Packet(struct packet *one_packet, int fd)
{
    struct packet socket_packet;
    socket_packet.packetType = RESPONSE;
    socket_packet.type = CORRECT;
    memset(socket_packet.data,0,MAXDATALEN);
    pthread_mutex_lock(&mutex);
    int j = 0;
    for (int i = 0; i < LISTENQUEUESIZE; ++i)
    {
        if (list_client[i].fd > 0)
        {
            /*
                include: fd + port + addr
            */
            j+=sprintf(socket_packet.data+j, "(MiniServer) Socketfd:%d ", list_client[i].fd);
            j+=sprintf(socket_packet.data+j, "Port::%hu ", list_client[i].port);
            j+=sprintf(socket_packet.data+j, "IP:%s\n", inet_ntoa(list_client[i].addr));
        }
    }
    pthread_mutex_unlock(&mutex);
    send(fd, (char *)&socket_packet, sizeof(socket_packet), 0);
}



