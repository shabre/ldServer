//
//  benchmark.cpp
//  ldServer
//
//  Created by Shabre on 2017. 11. 18..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "benchmark.hpp"

benchmark::benchmark(){
    curConnected=0;
    avgConnected=0.0;
    disconnect=0;
    connect=0;
    count=1;
    sumConnect=0;
}

void benchmark::incCur(){
    curConnected+=1;
}

void benchmark::dcsCur(){
    curConnected-=1;
}

void benchmark::incConnect(){
    connect+=1;
}

void benchmark::incDisconnect(){
    disconnect+=1;
}

long long benchmark::getConnected(){
    return connect;
}

int benchmark::getCurConnected(){
    return curConnected;
}

long long benchmark::getDisconnected(){
    return disconnect;
}

double benchmark::getAvgConnected(){
    return avgConnected;
}

void benchmark::updateAvg(){
    sumConnect+=curConnected;
    avgConnected=sumConnect/count;
    count+=1;
}
