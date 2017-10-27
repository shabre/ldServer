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
        public GameObject otherPlayer, emptyplayer;
        public List<int> unitList=new List<int>();
        public Queue<int> delQueue=new Queue<int>();
        public UnitPos[] unitArray;
        public GameObject[] playerPool;
        private Boolean[] online;
        AnalyzeBuf analyzeBuf;
	void Start () {
                analyzeBuf=new AnalyzeBuf();
                playerPool=new GameObject[1500];
                unitArray=new UnitPos[10000];
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
                
                String id = "Player1001";
                len+=(short)(id.Length);

                Pos_Packet pPacket = new Pos_Packet(request,len,
                        xPos,yPos,zPos,id);

                
                byte[] sbuf=pPacket.packetsToByte();
                client.Send(sbuf);
                /* 
                foreach(UnitPos unit in unitList){
                        Debug.Log(unit.ID);
                }*/
                
                updatePlayer(unitList,delQueue);
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
                while((nbyte=client.Receive(rbuf))>0){
                        //Pos_Packet rPacket=new Pos_Packet(rbuf);
                        //Debug.Log(nbyte+" byte received");
                        //Debug.Log("pQueue Size: "+pQueue.Count);
                        analyzeBuf.bufToPacket(rbuf,nbyte,ref pQueue,ref tQueue, ref delQueue);
                        analyzeBuf.handleOther(ref unitList, ref unitArray); 
                }
        }

        public int getPlayerCode(String id){
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



        public void updatePlayer(List<int> unitList, Queue<int> delQueue){
                //Debug.Log("unitlist size:"+unitList.Count);
                int delUnit;
                int index=0;
                while(delQueue.Count>0){
                        delUnit=delQueue.Dequeue();
                        
                        if(delUnit!=1001){
                                if(online[delUnit]){
                                        online[delUnit]=false;
                                        analyzeBuf.deleteOnline(delUnit);//handleother script의 오프라인 설정
                                        Destroy(playerPool[delUnit]);
                                        index=unitList.IndexOf(delUnit);
                                        unitList.RemoveAt(index);
                                }
                                else{
                                        
                                }
                        }
                        else{
                                Debug.Log("player server change");
                                
                                foreach(int unit in unitList){
                                        analyzeBuf.deleteOnline(unit);//handleother script의 오프라인 설정
                                        online[unit]=false;
                                        Destroy(playerPool[unit]);
                                }
                                unitList.Clear();     
                        }       
                }
        
                foreach(int unit in unitList){
                        Vector3 pos=new Vector3(unitArray[unit].xPos,unitArray[unit].yPos,unitArray[unit].zPos);
                        Quaternion rot=new Quaternion();
                        if(online[unit]){
                                playerPool[unit].transform.position=pos;
                                //playerPool[unit].transform.position=new Vector3((float)0.0,(float)0.0,(float)0.0);
                                /* 
                                if(playerPool[unit].transform.position.x<0){
                                        Destroy(playerPool[unit]);
                                        online[unit]=false;
                                        unitList.RemoveAt(index);
                                        index-=1;
                                }
                                */
                        }
                        else{
                                online[unit]=true;
                                playerPool[unit]=Instantiate(otherPlayer,pos,rot);
                                
                        }
                       index+=1;
                }
                

        }

        //어플리케이션 종료시켜야함
        public void quitApp(){
                Debug.Log("app quit");
                Application.Quit();
        }
}
