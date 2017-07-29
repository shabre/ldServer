//
//  analyzeBuf.cpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//
#include "analyzeBuf.h"

#define HSIZE 16

void bufToHeader(char *buf, struct Pos_packet *pPacket){
    memcpy(&(pPacket->request), buf, 2);
    memcpy(&(pPacket->dLength), buf+2, 2);
    memcpy(&(pPacket->xPos), buf+4, 4);
    memcpy(&(pPacket->yPos), buf+8, 4);
    memcpy(&(pPacket->zPos), buf+12, 4);
}

void packetToBuf(char *buf, struct Pos_packet *pPacket){
    memcpy(buf, &(pPacket->request), 2);
    memcpy(buf+2, &(pPacket->dLength), 2);
    memcpy(buf+4, &(pPacket->xPos), 4);
    memcpy(buf+8, &(pPacket->yPos), 4);
    memcpy(buf+12, &(pPacket->zPos), 4);
    memcpy(buf+16, pPacket->ID, pPacket->dLength);
}

//queue에 포인터로서 메모리를 저장하는 방식 **test완료 queueTest에서 테스트
void bufToPacket(char *buf, int nbyte, std::queue<Pos_packet *> *pQueue,
                               std::queue<char *> *tQueue){
    char *prev, *rest;//queue맨앞에는 잘린 패킷의 길이를 저장한다
    int restLen, idx=0;
    char headBuf[HSIZE];
    Pos_packet *pPacket;
    
    while(idx<nbyte){
        if(tQueue->size()!=0){//이전 수신한 buf가 아직 모두 처리되지 않았으면
            pPacket=(Pos_packet*)malloc(sizeof(struct Pos_packet));
            prev=tQueue->front();//temp queue에서 처리되지 않은 데이터를 꺼내온다
            tQueue->pop();
            
            if(prev[0]<HSIZE){//헤더가 잘렸을 경우
                restLen=HSIZE-prev[0];//잘린 헤더 크기를 알아낸후
                memcpy(headBuf, prev+1, prev[0]);//잘린부분을 복사
                memcpy(headBuf+prev[0], buf, restLen);//버퍼에있던 나머지부분을 복사
                idx=restLen;
                bufToHeader(headBuf, pPacket);
                pPacket->ID=(char *)malloc((int)pPacket->dLength+1);
                memcpy(pPacket->ID, buf+idx, pPacket->dLength);//버퍼의 나머지 데이터 부분을 가져옴
                idx+=pPacket->dLength;
            }
            else if(prev[0]==HSIZE){//헤더가 딱 템프에 들어갔을 경우
                memcpy(headBuf, prev+1, HSIZE);
                bufToHeader(headBuf, pPacket);
                pPacket->ID=(char *)malloc((int)pPacket->dLength+1);
                memcpy(pPacket->ID,buf,pPacket->dLength);
                idx+=pPacket->dLength;
            }
            else{//데이터가 잘렸을 경우
                memcpy(headBuf, prev+1, HSIZE);
                restLen=prev[0]-HSIZE;//템프에 남아있는 데이터 크기
                bufToHeader(headBuf, pPacket);
                pPacket->ID=(char *)malloc((int)pPacket->dLength+1);
                memcpy(pPacket->ID, prev+1+HSIZE, restLen);// 템프에 남아있는 데이터 복사
                memcpy(pPacket->ID+restLen, buf, pPacket->dLength-restLen);//버퍼에서의 나머지 보갓
                idx+=(pPacket->dLength-restLen);
            }
            free(prev);
        }
        else{
            if(nbyte-idx<HSIZE){//남은 처리해야하는 데이터에서 헤더가 잘렸을 경우
                rest=(char *)malloc(nbyte-idx+1);
                rest[0]=nbyte-idx;//queue 맨앞에 데이터길이
                memcpy(rest+1, buf+idx, (nbyte-idx));//버퍼에서 남은 부분을
                tQueue->push(rest);//큐에 넣는다
                idx=nbyte;
                rest=NULL;
                return;
            }
            else{
                pPacket=(Pos_packet*)malloc(sizeof(struct Pos_packet));
                memcpy(headBuf, buf+idx, HSIZE);
                idx+=HSIZE;
                bufToHeader(headBuf, pPacket);
                
                if(pPacket->dLength>nbyte-idx){//가져와야하는 데이터 양보다 버퍼에 남은 데이터양이 적을경우
                    free(pPacket);
                    idx-=HSIZE;
                    rest=(char *)malloc(nbyte-idx+1);
                    rest[0]=nbyte-idx;//queue 맨앞에 데이터길이
                    memcpy(rest+1, buf+idx, (nbyte-idx));//버퍼에서 남은 부분을
                    tQueue->push(rest);//큐에 넣는다
                    idx=nbyte;
                    rest=NULL;
                    return;
                }
                else{
                    pPacket->ID=(char *)malloc((int)pPacket->dLength+1);
                    memcpy(pPacket->ID, buf+idx, pPacket->dLength);
                    idx+=pPacket->dLength;
                }
            }
        }
        pQueue->push(pPacket);
        printf("ID: %s x: %f y: %f z: %f\n",pPacket->ID,pPacket->xPos,pPacket->yPos,pPacket->zPos);
    }
}

void sendPacket(int accp_sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue){
    char buf[512];
    struct Pos_packet *pPacket;
    if(pQueue->size()>0){
        pPacket=pQueue->front();
        pQueue->pop();
        printf("send %s x: %f y: %f z: %f\n",pPacket->ID, pPacket->xPos, pPacket->yPos, pPacket->zPos);
        packetToBuf(buf, pPacket);
        write(accp_sock, buf, HSIZE+pPacket->dLength);
    }
}
