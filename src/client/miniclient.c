#include "miniclient.h"


bool connect2Server(const char *ip,const char *port){
    
    int portId;
    bool ret = true;
    sscanf(port,"%d",&portId);
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == socketfd) {
        printf("(Client) Define socketfd failed!\n");
    }
    //printf("%s %d",ip,portId);
    //define socketaddr_in
    struct sockaddr_in inAddr;
    memset(&inAddr, 0, sizeof(inAddr));
    inAddr.sin_family = AF_INET;
    
    //set a fixed port to test   
    inAddr.sin_port = htons(4548);  //server port
    inAddr.sin_addr.s_addr = inet_addr(ip);  //server IP
    //connect
    if (connect(socketfd, (struct sockaddr *)&inAddr, sizeof(inAddr)) < 0)
        ret = false;

    return ret;
}

void sendDisRequestPacket(void) {
	packet pkt;
	pkt.pType = REQUEST;
	pkt.type = (int)DISCONNECT;
	memset(pkt.data, 0, sizeof(pkt.data));
	sprintf(pkt.data,"(MiniClient) DisConnect Request");
	send(socketfd, (char *)&pkt, sizeof(pkt), 0);
    //thread
    pthread_cancel(p);
}

void sendTimeRequestPacket(void) {
	packet pkt;
	pkt.pType = REQUEST;
	pkt.type = (int)TIME;
	memset(pkt.data, 0, sizeof(pkt.data));
	sprintf(pkt.data,"(MiniClient) Get Time Request");
	#ifdef ONEHUNDRED
	for(int i = 0;i<99;i++){
		send(socketfd, (char *)&pkt, sizeof(pkt), 0);
		usleep(50);
	}
	#endif
	send(socketfd, (char *)&pkt, sizeof(pkt), 0);
}

void sendNameRequestPacket(void) {
	packet pkt;
	pkt.pType = REQUEST;
	pkt.type = (int)NAME;
	memset(pkt.data, 0, sizeof(pkt.data));
	sprintf(pkt.data,"(MiniClient) Get Name Request");
	send(socketfd, (char *)&pkt, sizeof(pkt), 0);
}

void sendListRequestPacket(void) {
	packet pkt;
	pkt.pType = REQUEST;
	pkt.type = (int)LIST;
	memset(pkt.data, 0, sizeof(pkt.data));
	sprintf(pkt.data,"(MiniClient) Get List Request");
	send(socketfd, (char *)&pkt, sizeof(pkt), 0);
}

void sendMessageRequestPacket(const char *client,const char *msg) {
	int destClient;
	packet pkt;
	pkt.pType = REQUEST;
	pkt.type = (int)MESSAGE;
	memset(pkt.data, 0, sizeof(pkt.data));

	sscanf(client,"%d", &destClient);
	//printf("client id:%d\n",destClient);
	memcpy(pkt.data, &destClient, sizeof(int));

	strcat(pkt.data + sizeof(int),msg);


	send(socketfd, (char *)&pkt, sizeof(pkt), 0);

}
