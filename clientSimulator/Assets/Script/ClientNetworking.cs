using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using PacketProtocols;
using System.Threading;

//서버와의 네트워킹을 담당하는 클래스
public class ClientNetworking : MonoBehaviour {
 
        IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("203.249.75.14"), 52380);
        Transform tr;
        Socket client;
	void Start () {
                client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                client.Connect(ipep);
                ThreadStart ts=new ThreadStart(receivePacket);
                Thread tid=new Thread(ts);
                tid.Start();
	}
	
	// Update is called once per frame
	void Update () {
                short len=0;
                tr=this.gameObject.GetComponent<Transform>();
                
                short request =1;
                float xPos=tr.position.x;
                float yPos=tr.position.y;
                float zPos=tr.position.z;
                
                String id = "Player1";
                len+=(short)(id.Length);

                Pos_Packet pPacket = new Pos_Packet(request,len,
                        xPos,yPos,zPos,id);

                
                byte[] sbuf=pPacket.packetsToByte();
                client.Send(sbuf);
                /* 
                nbyte=client.Receive(rbuf);
                Pos_Packet rPacket=new Pos_Packet(rbuf);
                Debug.Log(nbyte+" byte received");
                Debug.Log(rPacket.getID()+" x: "+rPacket.getX()
                        +" y: "+rPacket.getY() +" z: "+rPacket.getZ());
                
                //client.Close();
                */
	}
        void onDestroy(){
                client.Close();
        }

        void receivePacket(){
                int nbyte;
                byte[] rbuf=new byte[512];
                while(true){
                        nbyte=client.Receive(rbuf);
                        Pos_Packet rPacket=new Pos_Packet(rbuf);
                        Debug.Log(nbyte+" byte received");
                        Debug.Log(rPacket.getID()+" x: "+rPacket.getX()
                                +" y: "+rPacket.getY() +" z: "+rPacket.getZ());
                }
                
                
        }
}
