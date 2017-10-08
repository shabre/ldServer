//
//  connect_loadBalancer.hpp
//  distributed server
//
//  Created by Shabre on 2017. 10. 7..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef connect_loadBalancer_hpp
#define connect_loadBalancer_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string>


int connect_ld(char *port);

#endif /* connect_loadBalancer_hpp */
