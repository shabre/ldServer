//
//  tcpServer.cpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "tcpServer.h"
#define MAXLINE 1023

void tcp_server(char *port ,std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue){
    struct sockaddr_in servaddr, cliaddr;
    int tcp_listen_sock, accp_sock, nbyte;
    socklen_t addrlen=sizeof(cliaddr);
    
    char buf[MAXLINE+1];
    if((tcp_listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail"); exit(0);
    }
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(port));
    // binding
    if(bind(tcp_listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind fail"); exit(0);
    }
    listen(tcp_listen_sock, 5);
    while(1) {
        puts("Waiting clients to connect.."); // 연결요청을 기다림
        accp_sock=accept(tcp_listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
        if(accp_sock < 0) {
            perror("accept fail");
            exit(0);
        }
        puts("Client connected..");
        while((nbyte = read(accp_sock, buf, MAXLINE))>0){//연결 끊길때까지 계속 수신
            std::cout<<nbyte<<" byte received"<<std::endl;
            bufToPacket(buf, nbyte, pQueue, tQueue);//수신한 데이터를 packet으로 변경
            sendPacket(accp_sock, pQueue, tQueue);//packet을 처리
        }
        close(accp_sock);
    }
    close(tcp_listen_sock);
}
