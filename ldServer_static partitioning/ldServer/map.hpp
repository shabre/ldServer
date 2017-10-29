//
//  map.hpp
//  dynamic partitioning
//
//  Created by Shabre on 2017. 10. 27..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef map_hpp
#define map_hpp

#include <iostream>
#include <list>


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
};

class map{
private:
    zone *myZone;
    int zoneNum;
    int serverNum;
    int *zoneServer;
public:
    map(int zoneNum, int serverNum);
    void serverShift(zone myZone, int beforeServer, int afterServer);
    void playerShift();
};

#endif /* map_hpp */
