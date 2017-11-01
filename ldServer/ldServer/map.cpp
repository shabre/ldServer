//
//  map.cpp
//  dynamic partitioning
//
//  Created by Shabre on 2017. 10. 27..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "map.hpp"

map::map(int numOfZone, int numOfServer){
    this->numOfZone=numOfZone;
    myZone=new zone[numOfZone]();
    zoneServer=(int*)malloc(sizeof(int)*numOfZone);//zoneServer 동적할당
    connectedZone=(int*)malloc(sizeof(int)*10000);
    for(int i=0; i<10000; i++){
        connectedZone[i]=-1;
    }
    for(int i=0; i<numOfZone; i++){
        myZone[i]=zone();//각 존의 플레이어 임의10000명
        zoneServer[i]=-1;//초기화
    }
    if(numOfServer==2){
        for(int i=0; i<numOfZone; i++){
            if(i<5)
                zoneServer[i]=0;
            else
                zoneServer[i]=1;
        }
    }
    else{
        printf("not implemented yet/n");
        exit(0);
    }
}

void map::init_dbalancer(int zoneNum, int serverNum, int playerWeight, int boundaryWeight, int closeBoundWeight){
    dBalancer=new dynamicBalancer(zoneNum, serverNum, playerWeight, boundaryWeight, closeBoundWeight);
}

int map::getConnectedZone(int hashVal){
    return connectedZone[hashVal];
}

void map::putConnectedZone(int zoneNum, int playerCode){
    connectedZone[playerCode]=zoneNum;
}

int map::getServerNum(int zoneNum){
    return zoneServer[zoneNum];
}

int map::getServerWeight(int serverNum){
    return dBalancer->getServerWeight(myZone, zoneServer, serverNum);
}

int map::getZoneWeight(int zoneNum){
    return dBalancer->getZoneWeight(myZone[zoneNum]);
}

int map::inputPlayerToZone(int zoneNum, int hashVal, float x, float y, float z){
    connectedZone[hashVal]=zoneNum;
    return myZone[zoneNum].inputPlayer(hashVal, x, y, z);
}

int zone::inputPlayer(int hashVal, float x, float y, float z){
    if(!online[hashVal]){//오프라인 상태였으면 새로운 정보를 입력한다
        online[hashVal]=true;
        playerPos[hashVal].x=x;
        playerPos[hashVal].y=y;
        playerPos[hashVal].z=z;
        
        playerList.push_back(hashVal);
        numOfMember+=1;
        return 0;//신규 등록 0 반환
    }
    else{//이미 접속한 상태라면 플레이어 위치 업데이트
        playerPos[hashVal].x=x;
        playerPos[hashVal].y=y;
        playerPos[hashVal].z=z;
        
        return 1;
    }
}

int zone::deletePlayer(int hashVal){
    online[hashVal]=false;
    numOfMember-=1;
    std::list<int>::iterator it;
    bool flag=false;
    for(it=playerList.begin(); it!=playerList.end(); it++){
        if((*it)==hashVal){
            flag=true;
            playerList.erase(it);
            break;
        }
    }
    
    if(flag)
        return 1;
    else
        return 0;
}

int zone::getNumOfMember(){
    return numOfMember;
}

zone::zone(){
    playerPos=(struct position *)malloc(sizeof(struct position)*10000);//zone의 player 관리 할당
    online=(bool *)malloc(sizeof(bool)*10000);
    numOfMember=0;
    for(int i=0; i<10000; i++)
        online[i]=false;
    
}

std::list<int> zone::getPlayerCodeList(){
    return playerList;
}

std::list<int> map::getZonePlayerCodeList(int zoneNum){
    return myZone[zoneNum].getPlayerCodeList();
}

void map::playerZoneShift(int beforeZoneNum, int afterZoneNum, int playerCode, float x, float y, float z){
    myZone[beforeZoneNum].deletePlayer(playerCode);
    myZone[afterZoneNum].inputPlayer(playerCode, x, y, z);
}

void map::serverShift(int zoneNum, int afterServer){
    zoneServer[zoneNum]=afterServer;
}

int map::getZoneNum(float x, float y, float z){
    if(z>50.0){//1,2,3
        if(x<-50.0){
            return 0;
        }
        else if(x>= -50.0 && x<50.0){
            return 1;
        }
        else{
            return 2;
        }
    }
    else if(z<=50.0 && z>-50.0){
        if(x<-50.0){
            return 3;
        }
        else if(x>= -50.0 && x<50.0){
            return 4;
        }
        else{
            return 5;
        }
    }
    else{
        if(x<-50.0){
            return 6;
        }
        else if(x>= -50.0 && x<50.0){
            return 7;
        }
        else{
            return 8;
        }
    }
}

void map::printServerOccup(){
    if(numOfZone==9){
        std::cout<<"===server occupation==="<<std::endl;
        for(int i=0; i<numOfZone; i++){
            std::cout<<zoneServer[i]<<" ";
            if(i%3==2)
                std::cout<<std::endl;
        }
        std::cout<<"======================="<<std::endl<<std::endl;
    }
    else{
        std::cout<<"print server occupation not implemented yet"<<std::endl;
    }
    
}

int map::getBoundCost(int serverNum){
    return dBalancer->getServerBoundCost(serverNum, this->zoneServer);
}

int map::getShiftBoundCost(int srcServer, int dstServer, int zoneNum){
    return dBalancer->getAfterShiftBoundCost(srcServer, dstServer, zoneNum, this->zoneServer, this->myZone);
}

int map::getneighbor(int server, std::list<struct neighbor> *nList){
    int x,y;
    struct neighbor _neighbor;
    for(int i=0; i< numOfZone; i++){
        if(zoneServer[i]!=server){//내 서버가 아닌경우
            x=i/3;
            y=i%3;
            if(x-1>=0){
                if(zoneServer[(x-1)*3+y]!=server){
                    _neighbor.serverNum=zoneServer[i];
                    _neighbor.zoneNum=i;
                    nList->push_back(_neighbor);
                }
            }
            if(x+1<3){
                if(zoneServer[(x+1)*3+y]!=server){
                    _neighbor.serverNum=zoneServer[i];
                    _neighbor.zoneNum=i;
                    nList->push_back(_neighbor);
                }
            }
            if(y-1>=0){
                if(zoneServer[x*3+y-1]!=server){
                    _neighbor.serverNum=zoneServer[i];
                    _neighbor.zoneNum=i;
                    nList->push_back(_neighbor);
                }
            }
            if(y+1<3){
                if(zoneServer[x*3+y+1]!=server){
                    _neighbor.serverNum=zoneServer[i];
                    _neighbor.zoneNum=i;
                    nList->push_back(_neighbor);
                }
            }
        }
    }
    return nList->size();
}

dynamicBalancer::dynamicBalancer(int numOfZone, int numOfServer, int playerWeight, int boundaryWeight, int closeBoundWeight){
    this->numOfZone=numOfZone;
    this->numOfServer=numOfServer;
    this->boundaryWeight=boundaryWeight;
    this->playerWeight=playerWeight;
    this->closeBoundWeight=closeBoundWeight;
    
    this->serverWeight=new int[numOfServer];
    this->zoneWeight=new int[numOfZone];

}

int dynamicBalancer::getZoneWeight(zone myZone){
    return myZone.getNumOfMember()*playerWeight;
}

int dynamicBalancer::getServerWeight(zone *myZone, int *zoneServer, int serverNum){
    int serverWeight=0;
    for(int i=0; i<numOfZone; i++){
        if(zoneServer[i]==serverNum){
            serverWeight+=getZoneWeight(myZone[i]);
        }
    }
    return serverWeight;
}

int dynamicBalancer::getShiftWeight(int zoneNum, int srcServer, int dstServer){
    int weight=1;
    std::cout<<"not implemented yet"<<std::endl;
    return weight;
}

//1-동 2-서 4-남 8-북 가능 비트연산
int dynamicBalancer::checkShiftable(int srcServer, int dstServer, int zoneNum, int *zoneServer){
    int count=0;
    int x=zoneNum/3;
    int y=zoneNum%3;
    int able=0;
    if(numOfZone==9){
        int zone[3][3];
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                zone[i][j]=zoneServer[count];
                count+=1;
            }
        }
        if(x-1>=0){
            if(zone[x-1][y]==dstServer){
                able|=WEST;
            }
        }
        if(x+1<3){
            if(zone[x+1][y]==dstServer){
                able|=EAST;
                
            }
        }
        if(y-1>=0){
            if(zone[x][y-1]==dstServer){
                able|=NORTH;
            }
        }
        if(y+1<3){
            if(zone[x][y+1]==dstServer){
                able|=SOUTH;
            }
        }
    }
    else{
        std::cout<<"not implemented yet\n";
    }
    return able;
}

int dynamicBalancer::getServerBoundCost(int server, int *zoneServer){
    int x,y;
    int serverBound=0;
    if(numOfZone==9){
        int zone[3][3];
        for(int i=0; i<numOfZone; i++){
            x=i/3;
            y=i%3;
            zone[x][y]=zoneServer[i];
        }
            
        for(int i=0; i<numOfZone; i++){
            x=i/3;
            y=i%3;
            if(x-1>=0){
                if(zone[x][y]==server && zone[x-1][y]!=zone[x][y])
                    serverBound+=1;
            }
            if(x+1<3){
                if(zone[x][y]==server && zone[x+1][y]!=zone[x][y])
                    serverBound+=1;
            }
            if(y-1>=0){
                if(zone[x][y]==server && zone[x][y-1]!=zone[x][y])
                    serverBound+=1;
            }
            if(y+1<3){
                if(zone[x][y]==server && zone[x][y+1]!=zone[x][y])
                    serverBound+=1;
            }
        }
        
    }
    else{
        std::cout<<"not implemented yet\n";
    }
    return serverBound;
}

//Zone을 다른서버에 넘길 때 서버의 경계 값을 반환해줌
int dynamicBalancer::getAfterShiftBoundCost(int srcServer, int dstServer, int zoneNum, int *zoneServer, zone *myZone){
    
    int x,y,shiftable, cost, min=0;
    int boundCost=16;
    shiftable=0;
    cost=0;
    x=zoneNum/3;
    y=zoneNum%3;
    if(srcServer==zoneServer[zoneNum]){//이전하려는 서버일 경우
        shiftable=checkShiftable(srcServer, dstServer, zoneNum, zoneServer);
        //개방되는 방향의 개수만큼 코스트에 이득을 봄
        if((shiftable & NORTH) !=0){
            boundCost/=2;
        }
        if((shiftable & SOUTH) !=0){
            boundCost/=2;
        }
        if((shiftable & EAST) !=0){
            boundCost/=2;
        }
        if((shiftable & WEST) !=0){
            boundCost/=2;
        }
    }
    cost+=getZoneWeight(myZone[zoneNum]);
    cost+=boundCost*10;
    
    return cost;
    /*
    int count=0;
    int x=zoneNum/3;
    int y=zoneNum%3;
    int able=0;
   
    int serverBound[10]={0,};
    if(numOfZone==9){
        int zone[3][3];
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                zone[i][j]=zoneServer[count];
            }
        }
        zone[x][y]=dstServer;
        
        for(int i=0; i<numOfZone; i++){
            x=i/3;
            y=i%3;
            if(x-1>=0){
                if(zone[x-1][y]!=zone[x][y])
                    serverBound[zone[x][y]]+=1;
            }
            if(x+1<3){
                if(zone[x+1][y]!=zone[x][y])
                    serverBound[zone[x][y]]+=1;
            }
            if(y-1>=0){
                if(zone[x][y-1]!=zone[x][y])
                    serverBound[zone[x][y]]+=1;
            }
            if(y+1<3){
                if(zone[x][y+1]!=zone[x][y])
                    serverBound[zone[x][y]]+=1;
            }
        }
    }
    else{
        std::cout<<"not implemented yet\n";
    }
    return serverBound[dstServer];
     */
}

