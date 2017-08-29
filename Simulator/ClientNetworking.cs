using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using PacketProtocols;

//서버와의 네트워킹을 담당하는 클래스
public class ClientNetworking {
 
        IPEndPoint ipep;
        Socket client;
        Thread tid;
        Queue<byte[]> pQueue, tQueue;

        PlayerMove player=new PlayerMove();
        //
	public void connect() {
                ipep = new IPEndPoint(IPAddress.Parse("27.1.242.15"), 52380);
                client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                client.Connect(ipep);
                pQueue=new Queue<byte[]>();
                tQueue=new Queue<byte[]>();
                ThreadStart ts=new ThreadStart(receivePacket);
                tid=new Thread(ts);
                tid.Start();
	}
	

	// Update is called once per frame
	public void frameSend (String myId) {
                while(true){
                        Thread.Sleep(100);
                        
                        short len=0;
                        short request =1;
                        player.NextPosition();
                        float xPos=player.getX();
                        float yPos=player.getY();
                        float zPos=player.getZ();
                        String id = myId;
                        len+=(short)(id.Length);

                        Pos_Packet pPacket = new Pos_Packet(request,len,
                                xPos,yPos,zPos,id);

                     
                        byte[] sbuf=pPacket.packetsToByte();
                        client.Send(sbuf);
                }
	}
        public void disconnect(){
                client.Dispose();
        }

        //threading으로 패킷을 수신한다. 수신된 패킷은 AnalyzeBuf를 통해 처리된다.
        void receivePacket(){
                int nbyte;
                byte[] rbuf=new byte[512];
                AnalyzeBuf analyzeBuf=new AnalyzeBuf();
                while((nbyte=client.Receive(rbuf))>0){
                        //Pos_Packet rPacket=new Pos_Packet(rbuf);
                        //Console.WriteLine(nbyte+" byte received");
                        analyzeBuf.bufToPacket(rbuf,nbyte,ref pQueue,ref tQueue);
                }
        }
}
