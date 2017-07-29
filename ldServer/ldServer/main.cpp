//
//  main.cpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 16..
//  Copyright © 2017년 Shabre. All rights reserved.

#include <stdio.h>
#include <iostream>
#include "analyzeBuf.h"
#include "packetProtocol.h"
#include "tcpServer.h"

#define MAXLINE 1023



int main(int argc, char * argv[]) {
    std::queue<struct Pos_packet*> pQueue;
    std::queue<char *> tQueue;
    tcp_server(argv[1], &pQueue, &tQueue);
    return 0;
}
