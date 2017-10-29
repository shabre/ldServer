//
//  map.hpp
//  dynamic partitioning
//
//  Created by Shabre on 2017. 10. 27..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef map_hpp
#define map_hpp
#define EAST 1
#define WEST 2
#define SOUTH 4
#define NORTH 8

#include <iostream>
#include <list>
#include <cmath>

class dynamicBalancer;

struct position{
    float x;
    float y;
    float z;
};

class zone{
private:
    int numOfMember;
    struct position *playerPos;
    bool *online;
    std::list<int> playerList;
public:
    zone();
    int getNumOfMember();
    int inputPlayer(int hashVal, float x, float y, float z);
    int deletePlayer(int hashVal);
    std::list<int> getPlayerCodeList();
};

class map{
private:
    zone *myZone;
    int numOfServer;
    int numOfZone;
    int *zoneServer;
    int *connectedZone;
    dynamicBalancer *dBalancer;
public:
    map(int zoneNum, int serverNum);
    void init_dbalancer(int numOfZone, int numOfServer, int playerWeight, int boundaryWeight, int closeBoundWeight);
    void serverShift(zone myZone, int beforeServer, int afterServer);//서버를 통째로 다른 존으로 이동
    void playerZoneShift(int beforeZoneNum, int afterZoneNum, int playerCode, float x, float y, float z);//존의 플레이어 하나를 다른존으로 이동
    int getZoneNum(float x, float y, float z);
    void putConnectedZone(int zoneNum, int playerCode);
    void printServerOccup();
    int getServerNum(int zoneNum);
    int getConnectedZone(int hashVal);
    int inputPlayerToZone(int zoneNum, int hashVal, float x, float y, float z);
    int getServerWeight(int serverNum);
    int getZoneWeight(int zoneNum);
    std::list<int> getZonePlayerCodeList(int zoneNum);
    int getBoundCost(int serverNum);
    int getShiftBoundCost(int srcServer, int dstServer, int zoneNum);
};

class dynamicBalancer{
private:
    int numOfZone;
    int numOfServer;
    int boundaryWeight;
    int playerWeight;
    int closeBoundWeight;
    int *serverWeight;
    int *zoneWeight;
    int zoneSimul[5][5];
public:
    dynamicBalancer(int numOfZone, int numOfServer, int playerWeight, int boundaryWeight, int closeBoundWeight);
    int getZoneWeight(zone myZone);
    int getServerWeight(zone *myZone, int *zoneServer, int serverNum);
    int getShiftWeight(int zoneNum, int srcServer, int dstServer);
    
    int checkShiftable(int srcServer, int dstServer, int zoneNum, int *zoneServer);
    int getServerBoundCost(int server, int *zoneServer);
    int getAfterShiftBoundCost(int srcServer, int dstServer, int zoneNum, int *zoneServer, zone *myZone);
};

#endif /* map_hpp */
