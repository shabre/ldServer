using System.Threading;
using System;
class Simulator_main
{
    public static void serverConnect(object arg){
        int num=(int)arg;
        ClientNetworking cn=new ClientNetworking();
        cn.connect();
        cn.frameSend("simulator_"+num);
        cn.disconnect();
    }

    public static void Main(string[] args){
        
        int multi=200;

        ParameterizedThreadStart[] ts=new ParameterizedThreadStart[multi];
        Thread[] tid=new Thread[multi];

        for(int i=0; i<multi; i++){
            ts[i] = new ParameterizedThreadStart(serverConnect);
            tid[i]=new Thread(ts[i]);
            tid[i].Start(i+1);
            Thread.Sleep(100);
            Console.WriteLine(i+" simulator connected");
        }
        

        for(int i=0; i<multi; i++){
            tid[i].Join();
            Console.WriteLine(i+" simulator disconnected");
        }
        
        
        //쓰레드로 송신시 제대로 송신 안되고, 서버에서도 문제가 생김.
        //일일히 패킷 뜯어서 상태 검사 해야할 듯 함
        //8.26
        
        /* //단일스레드
        ClientNetworking cn=new ClientNetworking();
        cn.connect();
        cn.frameSend("simulator_1");
        */
    }
}