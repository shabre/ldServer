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
#define DIMENSION 3
#define ITERATION 100

int dServer_sock[10];//분산 서버 소켓
int connectedServer[10000];//플레이어가 접속된 분산 서버
pthread_t tid[10000];
bool thread_usable[10000];
std::list<int> player_sock_list[10];
std::list<int> connectedClient;
int client_sock;
int numOfDS;
map *myMap;
KMeans *kmeans;
benchmark *bench;

pthread_mutex_t mut;
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
    int myServer=-1;//연결된 분산서버
    struct t_arg args=*((struct t_arg *)arg);
    pthread_mutex_unlock(&mut);
    int nbyte;
    struct Pos_packet *packet;
    char buf[MAXLINE+1];
    int hashID, xPos, yPos, zPos;
    puts("Client connected");
    std::cout<<"socket no: "<<args.accept_socket<<std::endl;
    std::queue<struct Pos_packet *> pQueue, dQueue;
    std::queue<char *> tQueue;
    std::vector<float> values;
    while((nbyte = read(args.accept_socket, buf, MAXLINE))>0){//연결 끊길때까지 계속 수신
        //std::cout<<nbyte<<" byte received"<<std::endl;
        bufToPacket(buf, nbyte, &pQueue, &tQueue);//수신한 데이터를 packet으로 변경
        //sendPacket부분을 분산서버에게 전송하는 형태로 변경
        //10.7 모든 클라에게 보내는 알고리즘을 접속된 분산 서버에 패킷을 전송하는 형식으로 변경해야함
        
        
        while(pQueue.size()>0){//패킷이 정상적으로 생성되었다면
            hashID=getHashID(pQueue.front()->ID);
            xPos=pQueue.front()->xPos;
            yPos=pQueue.front()->yPos;
            zPos=pQueue.front()->zPos;
            values.push_back(xPos); values.push_back(yPos); values.push_back(zPos);
            
            Point point(hashID, values, "simulator");
            
            if(myServer==-1){//분산 서버가 정해지지 않았다면
                //위치에 따른 조건을 거쳐 분산서버를 정해야함
                if(strncmp(pQueue.front()->ID,"Player",6)==0)
                    client_sock=args.accept_socket;
                //myServer=getRightServer(xPos, yPos, zPos, numOfDS);//임시로 0번 서버로 배정
                myServer=kmeans->getIDNearestCenter(point);
                printf("player%d connected to the server %d\n",hashID ,myServer);
                player_sock_list[myServer].push_back(args.accept_socket);//클라이언트 소켓넘버 저장
                connectedServer[hashID]=myServer;//클라이언트 해시에 분산서버 저장
                connectedClient.push_back(hashID);
                myMap->updateOnline(hashID);
                bench[myServer].incConnect();
                bench[myServer].incCur();
            }
            else{//현재 클라이언트가 다른 서버로 이전해야하는지에 대한 여부 판단
                myServer=connectedServer[hashID];
                
                //if(getRightServer(xPos, yPos, zPos, numOfDS)!=myServer){//새로 들어온 위치가 다른서버의 위치일 경우
                if(kmeans->getIDNearestCenter(point)!=myServer){//새로 들어온 위치가 다른서버의 위치일 경우
                    
                    //삭제 메세지 생성
                    packet=(Pos_packet *)malloc(sizeof(Pos_packet));
                    packet->ID=(char *)malloc(strlen(pQueue.front()->ID));
                    strcpy(packet->ID, pQueue.front()->ID);
                    packet->request=DELETE;
                    packet->xPos=0;packet->yPos=0;packet->zPos=0;
                    packet->dLength=strlen(packet->ID);
                    dQueue.push(packet);
                    bench[myServer].incDisconnect();
                    bench[myServer].dcsCur();
                    
                    if(client_sock!=0)
                        sendPlayerDelete(client_sock, packet);// 플레이어에게 해당 유저 제거 패킷전송
                    sendPacket(dServer_sock[myServer], &dQueue, &tQueue);
                    /*
                    printf("client %s server %d changed to %d\n"
                           ,pQueue.front()->ID,myServer, kmeans->getIDNearestCenter(point));
                     */
                            //,pQueue.front()->ID,myServer, getRightServer(xPos, yPos, zPos, numOfDS));
                    
                    //myServer=getRightServer(xPos, yPos, zPos, numOfDS);
                    myServer=kmeans->getIDNearestCenter(point);
                    connectedServer[hashID]=myServer;
                    bench[myServer].incCur();
                    bench[myServer].incConnect();
                    
                    packet=NULL;
                }
                myMap->updatePos(hashID, xPos, yPos, zPos);
                sendPacket(dServer_sock[myServer], &pQueue, &tQueue);
            }
            values.clear();
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
    pthread_mutex_unlock(&mut);

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

void *kmeans_clustering(void *arg){
    std::vector<Point> points;
    std::list<int>::iterator it;
    struct position pos;
    int dServer=*((int *)arg);
    
    while (1) {
        sleep(5);
        kmeans->reset_params(myMap->numOfClient(), DIMENSION, ITERATION);
        kmeans->reset_clusterPoints();
        
        for(it=connectedClient.begin(); it!=connectedClient.end(); it++){
            pos=myMap->getClientPos((*it));
            std::vector<float> val;
            val.push_back(pos.x);
            val.push_back(pos.y);
            val.push_back(pos.z);
            
            Point p((*it), val, "simulator");
            points.push_back(p);
        }
        if(points.size()>=numOfDS)
            kmeans->run(points);
        points.clear();
        
        for(int i=0; i<dServer; i++){
            bench[i].updateAvg();
            std::cout<<"server "<<i<<" connected:"<<bench[i].getCurConnected()<<" avg connection: "<<bench[i].getAvgConnected()<<
            " shift: "<<bench[i].getConnected()+bench[i].getDisconnected()<<std::endl;
        }
    }
    
    pthread_exit(NULL);
}

void tcp_server(int port, int numOfServer){
    numOfDS=numOfServer;
    bench= new benchmark[4];
    myMap=new map();
    pthread_mutex_init(&mut, NULL);
    pthread_mutex_init(&r_mutex, NULL);
    for(int i=0; i<numOfServer; i++){
        dServer_sock[i]=connect_dServer(port+i+1);//분산 서버와 연결
        printf("distributed Server %d connected..\n",port+i+1);
    }
    
    kmeans = new KMeans(numOfServer, numOfServer, DIMENSION, ITERATION);
    
    
    pthread_t tid_server[10];
    pthread_t kid;
    int serverNum[numOfDS];
    for(int i=0; i<numOfServer; i++){
        pthread_mutex_lock(&mut);
        serverNum[i]=i;
        pthread_create(&tid_server[i], NULL, receive_dServer, &serverNum[i]);
    }
    
    pthread_create(&kid, NULL, kmeans_clustering, &numOfServer);
    
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
            pthread_mutex_lock(&mut);
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
