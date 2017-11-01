using System.Collections;
using System.Collections.Generic;
using System;
using System.Net;
using System.Net.Sockets;
using PacketProtocols;

//이 클래스는 수신된 버퍼를 분석하여 패킷으로 변환해주는 클래스이다.
//분석된 패킷들은 ClientNetworking queue에 삽입이 된다.
//분석이 완료된 패킷들은 clientNetworking class에서 처리가 된다
public class AnalyzeBuf{
    private const int HSIZE=16;
    public void bufToPacket(byte[] buf,int nbyte, ref Queue<byte[]> pQueue, ref Queue<byte[]> tQueue){
        int idx=0, restLen=0;
        Pos_Packet pPacket;
        short dataLen=0;
        byte[] headBuf=new byte[HSIZE];
        byte[] prev, data, rest, enqbuf;
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
                    if(buf.Length<dataLen-restLen){//버퍼의 길이가 붙여야되는 데이터길이보다 짧을때
                        rest=new byte[prev[0]+1+buf.Length];
                        rest[0]=(byte)(restLen+buf.Length);
                        Array.Copy(prev,1,rest,0,prev[0]);
                        Array.Copy(buf,0,rest,prev[0]+1,buf.Length);
                        tQueue.Enqueue(rest);
                    }
                    else{
                        data=new byte[dataLen];
                        Array.Copy(prev,1+HSIZE,data,0,restLen);//템프 버퍼에 남아있는 데이터 만큼을 복사
                        Array.Copy(buf,0,data,restLen,dataLen-restLen);//버퍼에서 복사해야할 나머지 데이터
                        pPacket=new Pos_Packet(headBuf,data);
                        idx+=dataLen-restLen;
                    }
                }
                //enqbuf=pPacket.packetsToByte();
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
                        data=new byte[dataLen];//오류??
                        Array.Copy(buf, idx, data, 0, dataLen);
                        idx+=dataLen;
                        pPacket=new Pos_Packet(headBuf,data);
                        //enqbuf=pPacket.packetsToByte();
                    }
                }
            }
            //pQueue.Enqueue(enqbuf);
            //Console.WriteLine(pPacket.getID()+" x: "+pPacket.getX()+" y: "+pPacket.getY() +" z: "+pPacket.getZ());
        }
    }
    public void sendPacket(Socket client, ref Queue<byte[]> pQueue, ref Queue<byte[]> tQueue){
        byte[] buf;
        if(pQueue.Count>0){
            buf=(byte[])(pQueue.Dequeue());
            client.Send(buf);
        }
    }
}
