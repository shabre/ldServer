#include <iostream>
#include <list>
#include "connect_loadBalancer.hpp"
#include "packetProtocol.h"
#include "analyzeBuf.h"
#define HSIZE 16
#define MAXLINE 512
#define DELETE 2
char buf[512];
bool login[10000];//connected user
int countFrame;
using namespace std;

list<struct Pos_packet*> playerList;

void sendPacket(int accp_sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue){
    char buf[512];
    struct Pos_packet *pPacket;
    int hashval=0;
    if(pQueue->size()>0){
        pPacket=pQueue->front();
        countFrame+=1;
        hashval=getHashID(pPacket->ID);
        if(!login[hashval]){
            printf("%s connection complete\n", pPacket->ID);
            login[hashval]=true;
        }
        
       /* if(countFrame%60==0){
        printf("send %s req:%d x: %f y: %f z: %f\n",pPacket->ID,pPacket->request, pPacket->xPos, pPacket->yPos, pPacket->zPos);
            countFrame=0;
        }*/
         
        packetToBuf(buf, pPacket);
        if(write(accp_sock, buf, HSIZE+pPacket->dLength)<0)
            perror("write error");
        //printf("send length: %d\n", HSIZE+pPacket->dLength);
        pQueue->pop();
        free(pPacket->ID);
        free(pPacket);
    }
}

void deletePacket(int accp_sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue){
    struct Pos_packet *packet=pQueue->front();
    int hashval=getHashID(packet->ID);
    login[hashval]=false;
    //write(accp_sock, buf, HSIZE+packet->dLength);
    printf("%s disconnected from server\n", pQueue->front()->ID);
    pQueue->pop();
    free(packet->ID);
    free(packet);
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
        while(pQueue.size()>0){
            if(pQueue.front()->request==DELETE){//패킷이 삭제명령이면
                deletePacket(sock, &pQueue, &tQueue);
            }
            else if(pQueue.front()->request==1)
                sendPacket(sock, &pQueue, &tQueue);
            else{
                printf("req error: %d\n",pQueue.front()->request);
            }
        }
    }
    
    return 0;
}
