//
//  connect_loadBalancer.cpp
//  distributed server
//
//  Created by Shabre on 2017. 10. 7..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "connect_loadBalancer.hpp"

int connect_ld(char *port){
    struct sockaddr_in servaddr, cliaddr;
    int tcp_listen_sock, accp_sock;
    socklen_t addrlen=sizeof(cliaddr);
    
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
    listen(tcp_listen_sock, 10);

    accp_sock=accept(tcp_listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
    if(accp_sock < 0) {
        perror("accept fail");
        exit(0);
    }
    return accp_sock;
}
