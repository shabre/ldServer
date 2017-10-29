//
//  balancer.cpp
//  ldServer
//
//  Created by Shabre on 2017. 10. 7..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "balancer.hpp"
#define MAXLINE 1023
#define HSIZE 16

void sendPlayerDelete(int sock, struct Pos_packet* packet){
    char buf[MAXLINE+1];
    packetToBuf(buf, packet);
    if(write(sock, buf, HSIZE+packet->dLength)<0){
        perror("send delete message fail");
    }
    else{
        printf("delete message %d sent to %s\n",packet->request,packet->ID);
    }
}
