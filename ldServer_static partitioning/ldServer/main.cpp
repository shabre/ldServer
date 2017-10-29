//
//  main.cpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 16..
//  Copyright © 2017년 Shabre. All rights reserved.

#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "analyzeBuf.h"
#include "packetProtocol.h"
#include "tcpServer.h"

#define MAXLINE 1023



int main(int argc, char * argv[]) {
    std::queue<struct Pos_packet*> pQueue;
    std::queue<char *> tQueue;
    int numOfServer=atoi(argv[2]);
    
    int port=atoi(argv[1]);
    tcp_server(port, numOfServer);
    return 0;
}
