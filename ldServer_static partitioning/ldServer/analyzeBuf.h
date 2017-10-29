//
//  analyzeBuf.h
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef analyzeBuf_h
#define analyzeBuf_h

#include <queue>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "packetProtocol.h"

int getHashID(char *id);

//사이즈 16의 버퍼를 헤더로 변환시킨다
void bufToHeader(char *buf, struct Pos_packet *pPacket);

//패킷을 char 형태의 버퍼로 변환한다
void packetToBuf(char *buf, struct Pos_packet *pPacket);

//buf값을 packet으로 변환하여 queue에 저장시킨다
void bufToPacket(char *buf, int nbyte ,std::queue<struct Pos_packet *> *pQueue,
                               std::queue<char *> *tQueue);

void sendPacket(int accp_sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue);

#endif /* analyzeBuf_h */
