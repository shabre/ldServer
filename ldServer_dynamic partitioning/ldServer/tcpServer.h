//
//  tcpServer.hpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef tcpServer_h
#define tcpServer_h

#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <list>
#include "analyzeBuf.h"
#include "balancer.hpp"
#include "packetProtocol.h"
#include "static_partitioning.hpp"
#include "map.hpp"
#include "benchmark.hpp"

const char dServerIP[16]="127.0.0.1";

void tcp_server(int port, int numOfServer);
int getThreadID();
void sendPacket(int sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue);
void *connect_client(void *arg);
void *receive_dServer(void *arg);
int connect_dServer(int dServerPort);
void connect_client();

#endif /* tcpServer_h */
