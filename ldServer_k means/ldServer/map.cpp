//
//  map.cpp
//  dynamic partitioning
//
//  Created by Shabre on 2017. 10. 27..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "map.hpp"

map::map(){
    
}

void map::updatePos(int code, float x, float y, float z){
    pos[code].x=x;
    pos[code].y=y;
    pos[code].z=z;
}

void map::updateOnline(int code){
    connectedClient.push_back(code);
}

struct position map::getClientPos(int code){
    struct position ret;
    ret.x=pos[code].x;
    ret.y=pos[code].y;
    ret.z=pos[code].z;
    
    return ret;
}

int map::numOfClient(){
    return connectedClient.size();
}
