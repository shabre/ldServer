//
//  static_partitioning.cpp
//  ldServer
//
//  Created by Shabre on 2017. 10. 17..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#include "static_partitioning.hpp"

int getRightServer(int x, int y, int z, int part){
    if(part==0)
        return -1;
    else if(part==1){
        return 0;
    }
    else if(part==2){
        if(x<0)
            return 0;
        else
            return 1;
    }
    else if(part==3){
        printf("not implemented yet");
    }
    else{
        printf("not implemented yet");
        exit(0);
    }
    return -1;
}
