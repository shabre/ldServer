//
//  tcpServer.cpp
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//
#include "tcpServer.h"
#define MAXLINE 1023
#define HSIZE 16
#define DELETE 2


int dServer_sock[10];//분산 서버 소켓
int connectedServer[10000];//플레이어가 접속된 분산 서버
pthread_t tid[10000];
bool thread_usable[10000];
std::list<int> player_sock_list[10];
int client_sock;
int numOfDS;
int numOfZone;
map *myMap;
benchmark *bench;


pthread_mutex_t mutex;
pthread_mutex_t r_mutex;
struct t_arg{
    int accept_socket;
    int thread_num;
};

int getThreadID(){//사용가능한 thread id 검색
    for(int i=0; i<10000; i++){
        if(thread_usable[i]){
            thread_usable[i]=false;
            return i;
        }
    }
    return -1;
}

int connect_dServer(int dServerPort){
    struct sockaddr_in servaddr;
    int s;
    if((s=socket(PF_INET, SOCK_STREAM, 0))<0){
        perror("socket fail");
        exit(0);
    }
    
    //shclient 의 소켓주소 구조체 작성
    bzero((char *)&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    if(inet_pton(AF_INET, dServerIP, &servaddr.sin_addr)==0){
        puts("wrong IP address");
        exit(0);
    }//in_addr
    servaddr.sin_port=htons(dServerPort); //unsinged short
    
    //연결요청
    if(connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){//sockaddr_in -> sockaddr
        perror("connection fail");
        exit(0);
    }
    return s;
}

void sendPacket(int sock, std::queue<struct Pos_packet *> *pQueue, std::queue<char *> *tQueue){
    char buf[512];
    struct Pos_packet *pPacket;
    if(pQueue->size()>0){
        pPacket=pQueue->front();
        //printf("send %s x: %f y: %f z: %f\n",pPacket->ID, pPacket->xPos, pPacket->yPos, pPacket->zPos);
        packetToBuf(buf, pPacket);
        write(sock, buf, HSIZE+pPacket->dLength);
        pQueue->pop();
        free(pPacket->ID);
        free(pPacket);
    }
}

void *connect_client(void *arg){
    int connServer=-1;//연결된 분산서버
    int connZone=-1;//연결된 존
    int newZone=-1, newServer=-1;
    struct t_arg args=*((struct t_arg *)arg);
    pthread_mutex_unlock(&mutex);
    int nbyte;
    struct Pos_packet *packet;
    char buf[MAXLINE+1];
    puts("Client connected");
    std::cout<<"socket no: "<<args.accept_socket<<std::endl;
    std::queue<struct Pos_packet *> pQueue, dQueue;
    std::queue<char *> tQueue;
    while((nbyte = read(args.accept_socket, buf, MAXLINE))>0){//연결 끊길때까지 계속 수신
        //std::cout<<nbyte<<" byte received"<<std::endl;
        bufToPacket(buf, nbyte, &pQueue, &tQueue);//수신한 데이터를 packet으로 변경
        //sendPacket부분을 분산서버에게 전송하는 형태로 변경
        //10.7 모든 클라에게 보내는 알고리즘을 접속된 분산 서버에 패킷을 전송하는 형식으로 변경해야함
        
        
        while(pQueue.size()>0){//패킷이 정상적으로 생성되었다면
            
            /*
            if(myServer==-1){//분산 서버가 정해지지 않았다면
                //위치에 따른 조건을 거쳐 분산서버를 정해야함
                if(strncmp(pQueue.front()->ID,"Player",6)==0)
                    client_sock=args.accept_socket;
                myServer=getRightServer(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos, numOfDS);//임시로 0번 서버로 배정
                printf("player%d connected to the server %d\n",getHashID(pQueue.front()->ID),myServer);
                player_sock_list[myServer].push_back(args.accept_socket);//클라이언트 소켓넘버 저장
                connectedServer[getHashID(pQueue.front()->ID)]=myServer;//클라이언트 해시에 분산서버 저장
            }
            else{//현재 클라이언트가 다른 서버로 이전해야하는지에 대한 여부 판단
                myServer=connectedServer[getHashID(pQueue.front()->ID)];
                
                if(getRightServer(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos, numOfDS)!=myServer){//새로 들어온 위치가 다른서버의 위치일 경우
                    
                    //삭제 메세지 생성
                    packet=(Pos_packet *)malloc(sizeof(Pos_packet));
                    packet->ID=(char *)malloc(strlen(pQueue.front()->ID));
                    strcpy(packet->ID, pQueue.front()->ID);
                    packet->request=DELETE;
                    packet->xPos=0;packet->yPos=0;packet->zPos=0;
                    packet->dLength=strlen(packet->ID);
                    dQueue.push(packet);
                    
                    
                    if(client_sock!=0)
                        sendPlayerDelete(client_sock, packet);// 플레이어에게 해당 유저 제거 패킷전송
                    sendPacket(dServer_sock[myServer], &dQueue, &tQueue);
                    printf("client %s server %d changed to %d\n",pQueue.front()->ID,myServer, myServer=getRightServer(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos, numOfDS));
                    myServer=getRightServer(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos, numOfDS);
                    connectedServer[getHashID(pQueue.front()->ID)]=myServer;
                    
                    packet=NULL;
                }
                 
                sendPacket(dServer_sock[myServer], &pQueue, &tQueue);
            }
             */
            
            if(connZone==-1){//zone 할당이 안된 상태라면
                if(strncmp(pQueue.front()->ID,"Player",6)==0)
                    client_sock=args.accept_socket;
                connZone=myMap->getZoneNum(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos);//내 위치가 해당하는 zone num 얻어옴
                connServer=myMap->getServerNum(connZone);//zone에 해당하는 server를 가져온다
                printf("player%d connected to the server %d\n",getHashID(pQueue.front()->ID),connServer);
                player_sock_list[connServer].push_back(args.accept_socket);//클라이언트 소켓넘버 저장
                connectedServer[getHashID(pQueue.front()->ID)]=connServer;//클라이언트 해시에 분산서버 저장
                
                myMap->inputPlayerToZone(connZone, getHashID(pQueue.front()->ID), pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos);
                
                //benchmark
                bench[connServer].incConnect();
                bench[connServer].incCur();
            }
            
            else{//현재 클라이언트가 다른 zone으로 이동해야하는지 판단
                connZone=myMap->getConnectedZone(getHashID(pQueue.front()->ID));
                connServer=connectedServer[getHashID(pQueue.front()->ID)];
                if(myMap->getZoneNum(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos)
                   != connZone){//새로 갱신된 위치가 기존의 zone과 다른 위치의 zone 일시
                    
                    newZone=myMap->getZoneNum(pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos);
                    
                    if(myMap->getServerNum(connZone) != myMap->getServerNum(newZone)){//기존의 zone과 현재의 zone의 서버가 다른경우
                        //이 경우엔 zone 데이터 이동과 플레이어의 서버이전이 필요하다
                        //삭제 메세지 생성
                        /////////////////서버이전
                        packet=(Pos_packet *)malloc(sizeof(Pos_packet));
                        packet->ID=(char *)malloc(strlen(pQueue.front()->ID));
                        strcpy(packet->ID, pQueue.front()->ID);
                        packet->request=DELETE;
                        packet->xPos=0;packet->yPos=0;packet->zPos=0;
                        packet->dLength=strlen(packet->ID);
                        dQueue.push(packet);
                        
                        //benchmarking
                        bench[connServer].incDisconnect();
                        bench[connServer].dcsCur();
                        
                        
                        if(client_sock!=0)
                            sendPlayerDelete(client_sock, packet);// 플레이어에게 해당 유저 제거 패킷전송
                        sendPacket(dServer_sock[connServer], &dQueue, &tQueue);//기존 서버에 해당 유닛 삭제 명령
                        newServer=myMap->getServerNum(newZone);//새로 이전될 서버 가져옴
                        //printf("client %s server %d changed to %d\n",pQueue.front()->ID,connServer, newServer);
                        packet=NULL;
                        
                        //////////////////존 이전
                        myMap->playerZoneShift(connZone, newZone, getHashID(pQueue.front()->ID), pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos);
                        
 
                        connServer=newServer;
                        connectedServer[getHashID(pQueue.front()->ID)]=connServer;
                        bench[connServer].incCur();
                        bench[connServer].incConnect();
                        
                    }
                    else{//기존의 zone과 현재의 zone은 다르나 서버는 같은경우
                        //zone의 데이터 이동만 필요하다
                        myMap->playerZoneShift(connZone, newZone, getHashID(pQueue.front()->ID), pQueue.front()->xPos, pQueue.front()->yPos, pQueue.front()->zPos);
                    }
                    //printf("client %s zone %d changed to %d\n",pQueue.front()->ID,connZone, newZone);
                    connZone=newZone;
                    myMap->putConnectedZone(newZone, getHashID(pQueue.front()->ID));
                }
                sendPacket(dServer_sock[connServer], &pQueue, &tQueue);
            }
        }
        
        /*
         서버에 접속된 모든 클라이언트들에게 패킷을 전송하는 문장. 현재는 로드밸런싱 서버로 사용X
        for(it=accp_sock_list.begin(); it!=accp_sock_list.end(); it++){//접속해있는 모든 클라에게 전송
         sendPacket(*it , &pQueue, &tQueue);//packet을 처리
        }
         */
    }
    //accp_sock_list.remove(args.accept_socket);//전체접속 리스트에서 해당 소켓 제거
    close(args.accept_socket);
    pthread_exit(NULL);
}

void *receive_dServer(void *arg){
    int dServer=*((int *)arg);
    
    int tempS=dServer;
    int nbyte=0, myServer=-1;
    char buf[MAXLINE+1];
    char sbuf[MAXLINE+1];
    std::queue<struct Pos_packet *> pQueue;
    std::queue<char *> tQueue;
    struct Pos_packet *pPacket;
    std::list<int>::iterator it;
    pthread_mutex_unlock(&mutex);

    while(1){
        nbyte=read(dServer_sock[tempS], buf, MAXLINE);
        if(nbyte<0)
            break;
        //printf("server %d recv %d bytes\n",tempS, nbyte);
        //buf[nbyte]=0;
        bufToPacket(buf, nbyte, &pQueue, &tQueue);
        //std::cout<<"queue size: "<<pQueue.size()<<std::endl;
        while(pQueue.size()>0){
            pPacket=pQueue.front();
            myServer=connectedServer[getHashID(pPacket->ID)];
            //분산서버에 접속된 모든 플레이어들에게 해당 플레이어의 패킷을 전송한다
            //printf("send %s x: %f y: %f z: %f\n",pPacket->ID, pPacket->xPos, pPacket->yPos, pPacket->zPos);
            //std::cout<<"receiver size: "<<player_sock_list[myServer].size()<<std::endl;
            packetToBuf(sbuf, pPacket);
            
            /*//모든 해당되는 분산서버의 모든플레이어에게 패킷전송
            for(it=player_sock_list[myServer].begin(); it!=player_sock_list[myServer].end(); it++){
                if(write((*it), sbuf, HSIZE+pPacket->dLength)<0)
                    perror("dServer -> client fail");
             }
             */
            //플레이어에게만 모든 시뮬레이터의 정보 전송
            if(client_sock!=0){
                if(myServer==connectedServer[1001]){//클라이언트가 접속한 서버일 경우
                    if(write(client_sock, sbuf, HSIZE+pPacket->dLength)<0)//전송한다
                        perror("dServer -> client fail");
                }
                else{
                    
                }
            }
            pQueue.pop();
            free(pPacket->ID);
            free(pPacket);
        }
    }
    pthread_exit(NULL);
}

std::string intToString(int n)
{
    std::stringstream s;
    s << n;
    return s.str();
}

struct Pos_packet *createDeleteMsg(int code){
    std::string id;
    struct Pos_packet* packet=(Pos_packet *)malloc(sizeof(Pos_packet));
    if(code != 1001){
        id="simulator_"+intToString(code);
    }
    else{
        id="Player"+intToString(code);
    }
    packet->ID=(char *)malloc(id.length());
    strcpy(packet->ID,id.c_str());
    packet->request=DELETE;
    packet->xPos=0;packet->yPos=0;packet->zPos=0;
    packet->dLength=strlen(packet->ID);
    return packet;
}

void serverShift(int zone ,int prev, int next){
    struct Pos_packet *packet;
    std::queue<struct Pos_packet *> pQueue, dQueue;
    std::queue<char *> tQueue;
    std::list<int> codeList;
    std::list<int>::iterator it;
    
    codeList=myMap->getZonePlayerCodeList(zone);
    
    for(it=codeList.begin(); it!= codeList.end(); it++){
        connectedServer[(*it)]=next;
        packet=createDeleteMsg((*it));
        if(client_sock!=0)
            sendPlayerDelete(client_sock, packet);// 플레이어에게 해당 유저 제거 패킷전송
        dQueue.push(packet);
    }
    while(!dQueue.empty())
        sendPacket(dServer_sock[prev], &dQueue, &tQueue);//기존 서버에 해당 유닛 삭제 명령
    
    myMap->serverShift(zone, next);
    std::cout<<"zone "<< zone<<" server "<<prev<<" -> "<< next<<std::endl;
}

void *dynamicBalancing(void *arg){
    const int playerWeight=1;
    const int boundWeight=10;
    const int closeBound=3;
    int connServer;
    int max, min, maxServer, minServer, avg=0, minWeight=987654321, maxWeight=0;
    int weight;


    myMap->init_dbalancer(numOfZone, numOfDS, playerWeight, boundWeight , closeBound);
    
    int count=0;
    int total=0;
    while(true){
        max=0;
        min=987654321;
        
        sleep(5);//10초마다 밸런싱 시작
        printf("balancing strated...\n");
        std::list<struct neighbor> neighbors;
        std::list<struct neighbor>::iterator it, min_it;
        /*
         //server and zone print
        for(int i=0; i<numOfDS; i++){
            std::cout<<"server "<<i<<" weight : "<<serverWeight[i]<<std::endl;
        }
        for(int i=0; i<numOfZone; i++){
            std::cout<<"zone "<<i<<" weight : "<<myMap->getZoneWeight(i)<<std::endl;
        }
         */
    
        /*
        std::cout<<"bound cost server 0: "<<myMap->getBoundCost(0)<<" server 1: "<<myMap->getBoundCost(1)<<std::endl;
        std::cout<<"weight server 0: "<<myMap->getServerWeight(0)<<" server 1: "<<myMap->getServerWeight(1)<<std::endl;
        */
        
        for(int i=0; i<numOfDS; i++){
            bench[i].updateAvg();
            std::cout<<"server "<<i<<" connected:"<<bench[i].getCurConnected()<<" avg connection: "<<bench[i].getAvgConnected()<<
            " shift: "<<bench[i].getConnected()+bench[i].getDisconnected()<<std::endl;
        }
        
        //알고리즘
        //cost 기반
        //max가 존을 기부하고
        //min이 존을 다른곳에서 얻어오는형식
        //가까이 붙어있는 zone중 할당시 효율적인 경우를 계산해서 할당
        total=0;avg=0;
        for(int i=0; i<numOfDS; i++){//최대 탐색 알고리즘
            int weight=myMap->getServerWeight(i);
            int boundWeight=myMap->getBoundCost(i);
            int numOfZone=myMap->getNumOfZone(i);
            if(max<weight){
                max=weight;
                maxServer=i;
            }
            if(min>weight){
                min=weight;
                minServer=i;
            }
            total+=weight;
        }
        avg=total/numOfDS;
        
        std::cout<<"min: "<<min<<" max: "<<max<<" avg: "<<avg<<std::endl;
        
         /////////////////서버이전
        if(avg*1.5<max){
            minWeight=987654321;
            myMap->getneighbor(maxServer, &neighbors);
            for(it=neighbors.begin(); it!=neighbors.end(); it++){
                weight=myMap->getShiftBoundCost(maxServer, (*it).serverNum, (*it).zoneNum);
                weight*=(total/numOfZone);
                if(minWeight>weight){
                    minWeight=weight;
                    min_it=it;
                }
            }
            if(neighbors.size()>0){
                serverShift((*min_it).zoneNum, maxServer, (*min_it).serverNum);
                std::cout<<"max server shift complete"<<std::endl;
            }
        }
        neighbors.clear();
        if(min*1.5<avg){
            minWeight=987654321;
            myMap->getneighbor(minServer, &neighbors);
            for(it=neighbors.begin(); it!=neighbors.end(); it++){
                weight=myMap->getShiftBoundCost((*it).serverNum ,minServer, (*it).zoneNum);
                weight*=(total/numOfZone);
                if(minWeight>weight){
                    minWeight=weight;
                    min_it=it;
                }
            }
            if(neighbors.size()>0){
                serverShift((*min_it).zoneNum, (*min_it).serverNum, minServer);
                std::cout<<"min server shift complete"<<std::endl;
            }
        }
        myMap->printServerOccup();
        neighbors.clear();
    }
    pthread_exit(NULL);
}

void tcp_server(int port, int numOfServer){
    numOfDS=numOfServer;
    bench=new benchmark[numOfServer];
    numOfZone=9;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&r_mutex, NULL);
    for(int i=0; i<numOfServer; i++){
        dServer_sock[i]=connect_dServer(port+i+1);//분산 서버와 연결
        printf("distributed Server %d connected..\n",port+i+1);
    }
    
    myMap=new map(9,numOfServer);
    pthread_t tid_server[10];
    int serverNum[numOfDS];
    for(int i=0; i<numOfServer; i++){
        pthread_mutex_lock(&mutex);
        serverNum[i]=i;
        pthread_create(&tid_server[i], NULL, receive_dServer, &serverNum[i]);
    }
    
    pthread_t tid_balancer;
    pthread_create(&tid_balancer, NULL, dynamicBalancing, NULL);
    
    int avail;
    
    struct t_arg pass_arg;
    for(int i=0; i<10000; i++)
        thread_usable[i]=true;
    struct sockaddr_in servaddr, cliaddr;
    int tcp_listen_sock, accp_sock;
    socklen_t addrlen=sizeof(cliaddr);
    
    if((tcp_listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket fail"); exit(0);
    }
    bzero((char *)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    // binding
    if(bind(tcp_listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        perror("bind fail"); exit(0);
    }
    listen(tcp_listen_sock, 10);
    while(1) {
        //puts("Waiting clients to connect.."); // 연결요청을 기다림
        accp_sock=accept(tcp_listen_sock, (struct sockaddr *)&cliaddr, &addrlen);
        if(accp_sock < 0) {
            perror("accept fail");
            exit(0);
        }
      
        avail=getThreadID();//사용가능한 thread id 가져온다
        std::cout<<"thread id no: "<<avail<<std::endl;
        if(avail>=0){//사용가능한 thread id가 있으면
            pthread_mutex_lock(&mutex);
            pass_arg.accept_socket=accp_sock;
            pass_arg.thread_num=avail;
            pthread_create(&tid[avail], NULL, connect_client, &pass_arg);
            //pthread_join(tid[avail], NULL);
        }
        else{//사용가능한 thread id가 없으면
            perror("no avail thread");
            close(accp_sock);
            continue;
        }
    }
    close(tcp_listen_sock);
}
