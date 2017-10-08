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
        IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 52380);

        Transform tr;
        public GameObject myCharacter;
        Socket client, ipcSend;
        Thread tid;
        Queue pQueue, tQueue;
        public GameObject otherPlayer;
        public List<UnitPos> unitList=new List<UnitPos>();
        public GameObject[] playerPool;
        private Boolean[] online;
	void Start () {
                playerPool=new GameObject[1000];
                online=new Boolean[10000];
                for(int i=0; i<10000; i++)
                        online[i]=false;
                client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                client.Connect(ipep);
                pQueue=new Queue();
                tQueue=new Queue();
                ThreadStart ts=new ThreadStart(receivePacket);
                tid=new Thread(ts);
                tid.Start();
	}
	
	// Update is called once per frame
	void Update () {
                short len=0;
                tr=myCharacter.GetComponent<Transform>();
                
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
                foreach(UnitPos unit in unitList){
                        Debug.Log(unit.ID);
                }*/
                
                updatePlayer(unitList);
	}
        void onApplicationQuit(){
                //Debug.Log("----------------app end------------");
                client.Close();
                tid.Abort();
        }

        //threading으로 패킷을 수신한다. 수신된 패킷은 AnalyzeBuf를 통해 처리된다.
        void receivePacket(){
                int nbyte;
                byte[] rbuf=new byte[512];
                AnalyzeBuf analyzeBuf=new AnalyzeBuf();
                while((nbyte=client.Receive(rbuf))>0){
                        //Pos_Packet rPacket=new Pos_Packet(rbuf);
                        //Debug.Log(nbyte+" byte received");
                        analyzeBuf.bufToPacket(rbuf,nbyte,ref pQueue,ref tQueue);
                        analyzeBuf.handleOther(ref unitList);
                     
                }
        }

        private int getPlayerCode(String id){
        char[] number=new char[6];
        int code=0;
        int simLen="simulator_".Length;
        id.CopyTo(simLen, number, 0, id.Length-simLen);
        for(int i=0; i<id.Length-simLen; i++){
                if(i>0)
                        code*=10;
                code+=number[i]-48;
                }
                return code;		
        }



        public void updatePlayer(List<UnitPos> unitList){
                int code;

                foreach(UnitPos unit in unitList){
                        code=getPlayerCode(unit.ID);
                        Vector3 pos=new Vector3(unit.xPos,unit.yPos,unit.zPos);
                        Quaternion rot=new Quaternion();
                        if(online[code]){
                                playerPool[code].transform.position=pos;
                        }
                        else{
                                online[code]=true;
                                playerPool[code]=Instantiate(otherPlayer,pos,rot);
                        }
                }
        }

        //어플리케이션 종료시켜야함
        public void quitApp(){
                Debug.Log("app quit");
                Application.Quit();
        }
}
