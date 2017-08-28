using System.Collections;
using System.Collections.Generic;
using System;
using PacketProtocols;
using UnityEngine;
//다른 플레이어의 움직임을 확인한다
public class HandleOtherPlayer {
	private Boolean[] online;
	public List<GameObject> playerPool;
	public GameObject playerPrefab;
	public HandleOtherPlayer(){
		playerPool=new List<GameObject>();
		online=new Boolean[10000];
		for(int i=0; i<10000; i++){
			online[i]=false;
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

	public void setList(ref List<UnitPos> unitList, UnitPos player){
		Debug.Log("player code:" +getPlayerCode(player.ID));
		int index=0;
		if(online[getPlayerCode(player.ID)]){//이미 등록된 유저라면
			Debug.Log("online player: "+player.ID);
			foreach(UnitPos unit in unitList){
				if(unit.ID.Equals(player.ID))
					break;
				else
					index++;
			}
			unitList.RemoveAt(index);
			unitList.Add(player);
		}
		else{//등록되지 않은 유저라면
			unitList.Add(player);
			online[getPlayerCode(player.ID)]=true;
		}
	}
}
