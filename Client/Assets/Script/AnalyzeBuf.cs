using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using PacketProtocols;

//이 클래스는 수신된 버퍼를 분석하여 패킷으로 변환해주는 클래스이다.
//분석된 패킷들은 ClientNetworking queue에 삽입이 된다.
//분석이 완료된 패킷들은 clientNetworking class에서 처리가 된다
public class AnalyzeBuf{
    private const int HSIZE=16;
    private Queue otherPlayerQueue=new Queue();
    HandleOtherPlayer handleOtherPlayer=new HandleOtherPlayer();
    public void bufToPacket(byte[] buf,int nbyte, ref Queue pQueue, ref Queue tQueue){
        int idx=0, restLen=0;
        Pos_Packet pPacket;
        UnitPos unitPos;
        short dataLen=0;
        byte[] headBuf=new byte[HSIZE];
        byte[] prev, data, rest;
        while(idx<nbyte){//모든 패킷을 처리하기 전
            if(tQueue.Count!=0){//이전 수신한 buf가 아직 모두 처리되지 않았으면
                prev=(byte[])tQueue.Dequeue();
                if(prev[0]<HSIZE){//헤더가 잘렸을 경우
                    restLen=HSIZE-prev[0];//잘린 헤더 크기를 알아낸후
                    Array.Copy(prev,1,headBuf,0,prev[0]);//headBuf에 잘린부분 복사
                    Array.Copy(buf,0,headBuf,prev[0],restLen);//버퍼의 나머지 복사
                    idx=restLen;//나머지만큼 버퍼의 인덱스 증가
                    dataLen=BitConverter.ToInt16(headBuf, 2);//데이터 길이 알아냄
                    data=new byte[dataLen];
                    Array.Copy(buf,idx,data,0,dataLen);//버퍼에있는 데이터를 가져옴
                    pPacket=new Pos_Packet(headBuf,data);//패킷 생성
                    idx+=dataLen;//뽑아온 데이터만큼 인덱스 증가
                }
                else if(prev[0]==HSIZE){//헤더가 딱 temp queue에들어갔을 경우
                    Array.Copy(prev,1,headBuf,0,HSIZE);
                    dataLen=BitConverter.ToInt16(headBuf, 2);//데이터 길이 알아냄
                    data=new byte[dataLen];
                    Array.Copy(buf,0,data,0,dataLen);//버퍼에있는 데이터를 가져옴
                    pPacket=new Pos_Packet(headBuf,data);
                    idx+=dataLen;
                }
                else{
                    Array.Copy(prev,1,headBuf,0,HSIZE);
                    restLen=prev[0]-HSIZE;
                    dataLen=BitConverter.ToInt16(headBuf, 2);
                    data=new byte[dataLen];
                    Array.Copy(prev,1+HSIZE,data,0,restLen);//템프 버퍼에 남아있는 데이터 만큼을 복사
                    Array.Copy(buf,0,data,restLen,dataLen-restLen);//버퍼에서 복사해야할 나머지 데이터
                    pPacket=new Pos_Packet(headBuf,data);
                    idx+=dataLen-restLen;
                }
            }
            else{
                if(nbyte-idx<HSIZE){//처리해야하는 남은 데이터에서 헤더가 잘렸을 경우
                    rest=new byte[nbyte-idx+1];
                    rest[0]=(byte)(nbyte-idx);
                    Array.Copy(buf,idx,rest,1,nbyte-idx);
                    tQueue.Enqueue(rest);
                    idx=nbyte;
                    rest=null;
                    return;
                }
                else{
                    Array.Copy(buf, idx, headBuf, 0, HSIZE);
                    idx+=HSIZE;
                    dataLen=BitConverter.ToInt16(headBuf, 2);

                    if(dataLen>nbyte-idx){//가져와야하는 데이터 양보다 버퍼에 남은 데이터 양이 더 많을경우
                        idx-=HSIZE;
                        rest=new byte[nbyte-idx+1];
                        rest[0]=(byte)(nbyte-idx);
                        Array.Copy(buf, idx, rest, 1, nbyte-idx);
                        tQueue.Enqueue(rest);
                        idx=nbyte;
                        rest=null;
                        return;
                    }
                    else{
                        data=new byte[dataLen];
                        Array.Copy(buf, idx, data, 0, dataLen);
                        idx+=dataLen;
                        pPacket=new Pos_Packet(headBuf,data);
                        
                    }
                }
            }
            //게임 매니저 오브젝트의 updatePosition script를 가져와야함
            if(pPacket.getID()!="Player1"){
                unitPos=initUnitPos(pPacket.getID(),pPacket.getX(),pPacket.getY(),pPacket.getZ());
                otherPlayerQueue.Enqueue(unitPos);//otherplayerQueue에 입력
            }
            Debug.Log(pPacket.getID()+" x: "+pPacket.getX()
                                +" y: "+pPacket.getY() +" z: "+pPacket.getZ());
        }
    }

    public UnitPos initUnitPos(String ID, float x, float y, float z){
        UnitPos newUnit;
        newUnit.xPos=x;
        newUnit.yPos=y;
        newUnit.zPos=z;
        newUnit.ID=ID;
        return newUnit;
    }
    public void handleOther(ref List<UnitPos> unitList){
        Debug.Log("otherplayer queue size: "+otherPlayerQueue.Count);
        while(otherPlayerQueue.Count>0){
            handleOtherPlayer.setList(ref unitList, (UnitPos)otherPlayerQueue.Dequeue());
        }
    }
    public void sendPacket(Socket client, ref Queue pQueue, ref Queue tQueue){
        byte[] buf;
        if(pQueue.Count>0){
            buf=(byte[])(pQueue.Dequeue());
            client.Send(buf);
        }
    }
}
