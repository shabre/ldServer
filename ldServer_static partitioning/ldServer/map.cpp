//
//  map.cpp
//  dynamic partitioning
//
//  Created by Shabre on 2017. 10. 27..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "map.hpp"

map::map(int zoneNum, int serverNum){
    this->zoneNum=zoneNum;
    myZone=(zone *)malloc(sizeof(zone)*zoneNum);//zone 동적할당
    zoneServer=(int*)malloc(sizeof(int)*zoneNum);//zoneServer 동적할당
    for(int i=0; i<zoneNum; i++){
        myZone[i]=zone();//각 존의 플레이어 임의10000명
        zoneServer[i]=-1;//초기화
    }
    if(serverNum==2){
        for(int i=0; i<zoneNum; i++){
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

zone::zone(){
    playerPos=(struct position *)malloc(sizeof(struct position)*10000);//zone의 player 관리 할당
    online=(bool *)malloc(sizeof(bool)*10000);
    for(int i=0; i<10000; i++)
        online[i]=false;
    
}
