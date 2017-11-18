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

class map{
private:
    struct position pos[10000];//connected player's pos
    std::list<int> connectedClient;
public:
    map();
    void playerShift();
    void updatePos(int code, float x, float y, float z);
    void updateOnline(int code);
    int numOfClient();
    struct position getClientPos(int code);
};

#endif /* map_hpp */
