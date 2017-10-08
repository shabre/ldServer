//
//  packetProtocol.h
//  ldServer
//
//  Created by Shabre on 2017. 7. 20..
//  Copyright © 2017년 Shabre. All rights reserved.
//

#ifndef packetProtocol_h
#define packetProtocol_h


struct Pos_packet{
    short request; //요청내용
    short dLength; //데이터 길이
    float xPos;
    float yPos;
    float zPos;
    char *ID;
};
 

#endif /* packetProtocol_h */
