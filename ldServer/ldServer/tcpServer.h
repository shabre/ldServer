//
//  tcpServer.hpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef tcpServer_h
#define tcpServer_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <list>
#include "analyzeBuf.h"
#include "packetProtocol.h"

void tcp_server(char *port);

void connect_client();

#endif /* tcpServer_h */
