#include <iostream>
#include <list>
#include "connect_loadBalancer.hpp"
#include "packetProtocol.h"
#include "analyzeBuf.h"
#define HSIZE 16
#define MAXLINE 512
char buf[512];
bool login[10000];//connected user

using namespace std;

list<struct Pos_packet*> playerList;

void sendPacket(int accp_sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue){
    char buf[512];
    struct Pos_packet *pPacket;
    while(pQueue->size()>0){
        pPacket=pQueue->front();
        //printf("send %s x: %f y: %f z: %f\n",pPacket->ID, pPacket->xPos, pPacket->yPos, pPacket->zPos);
        packetToBuf(buf, pPacket);
        write(accp_sock, buf, HSIZE+pPacket->dLength);
        pQueue->pop();
        free(pPacket->ID);
        free(pPacket);
    }
}

bool alreadyIn(char *id){
    int hashval=getHashID(id);
    return login[hashval];
}

int main(int argc, char * argv[]) {
    int sock ,nbyte=0;
    if(argc<2){
        perror("input port number");
        return 0;
    }
    char sbuf[512];
    std::queue<struct Pos_packet *> pQueue;
    std::queue<char *> tQueue;
    struct Pos_packet *pPacket;
    sock=connect_ld(argv[1]);
    while((nbyte = read(sock, buf, MAXLINE))>0){
        buf[nbyte]=0;
        memset(sbuf, 0, 512);
        bufToPacket(buf, nbyte, &pQueue, &tQueue);
        if(pQueue.size()>0){
            sendPacket(sock, &pQueue, &tQueue);
        }
    }
    
    return 0;
}
