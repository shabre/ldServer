//
//  benchmark.hpp
//  ldServer
//
//  Created by Shabre on 2017. 11. 18..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef benchmark_hpp
#define benchmark_hpp

#include <stdio.h>

class benchmark{
private:
    double avgConnected;
    int curConnected;
    long long disconnect;
    long long connect;
    long long count;
    long long sumConnect;
    
public:
    benchmark();
    void incConnect();
    void incDisconnect();
    void incCur();
    void dcsCur();
    int getCurConnected();
    long long getConnected();
    long long getDisconnected();
    double getAvgConnected();
    void updateAvg();
};

#endif /* benchmark_hpp */
