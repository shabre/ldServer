using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System;

namespace PacketProtocols
{
	

	public struct UnitPos{
		public String ID;
		public float xPos;
		public float yPos;
		public float zPos;
	}

	//패킷의 구조
	public class Pos_Packet{
		public const short UPDATE=1;
		public const short DELETE=2;
        private short request; //요청내용
        private short dLength; //데이터 길이
		private float xPos;
		private float yPos;
		private float zPos;
        private String ID;
		
		public String getID(){
			return ID;
		}
		public float getX(){
			return xPos;
		}
		public float getY(){
			return yPos;
		}
		public float getZ(){
			return zPos;
		}

		public short getRequest(){
			return request;
		}

		//패킷의 첫 생성자
		public Pos_Packet(short req, short len,  
			float xp, float yp, float zp, String id){
			request=req;
			dLength=len;
			ID=id;
			xPos=xp;
			yPos=yp;
			zPos=zp;
		}

		//수신 바이트를 패킷으로 변환해주는 생성자
		public Pos_Packet(byte[] recv){
			request=BitConverter.ToInt16(recv, 0);
			dLength=BitConverter.ToInt16(recv, 2);
			xPos=BitConverter.ToSingle(recv, 4);
			yPos=BitConverter.ToSingle(recv, 8);
			zPos=BitConverter.ToSingle(recv, 12);
			Byte[] byteID=new byte[dLength];
			for(int i=0; i<dLength; i++){
				byteID[i]=recv[16+i];
			}
			ID=System.Text.Encoding.UTF8.GetString(byteID);
		}
		
		//수신된 바이트 스트림을 헤더와 데이터 따로 구분하여 패킷 생성
		public Pos_Packet(byte[] head, byte[] data){
			request=BitConverter.ToInt16(head, 0);
			dLength=BitConverter.ToInt16(head, 2);
			xPos=BitConverter.ToSingle(head, 4);
			yPos=BitConverter.ToSingle(head, 8);
			zPos=BitConverter.ToSingle(head, 12);
			ID=System.Text.Encoding.UTF8.GetString(data);
		}

		//패킷을 바이트 형태로 변환
		public Byte[] packetsToByte(){
			int size=0;
			byte[] req=BitConverter.GetBytes(this.request);
			byte[] len=BitConverter.GetBytes(this.dLength);
			byte[] x=BitConverter.GetBytes(this.xPos);
			byte[] y=BitConverter.GetBytes(this.yPos);
			byte[] z=BitConverter.GetBytes(this.zPos);
			byte[] idByte=Encoding.UTF8.GetBytes(this.ID);

			size+=sizeof(short)*2;
			size+=sizeof(float)*3;
			size+=idByte.Length;

			byte[] sPacket=new byte[size];
			size=0;
			Buffer.BlockCopy(req,0,sPacket,0,sizeof(short));
			size+=sizeof(short);
			Buffer.BlockCopy(len,0,sPacket,size,sizeof(short));
			size+=sizeof(short);
			Buffer.BlockCopy(x,0,sPacket,size,sizeof(float));
			size+=sizeof(float);
			Buffer.BlockCopy(y,0,sPacket,size,sizeof(float));
			size+=sizeof(float);
			Buffer.BlockCopy(z,0,sPacket,size,sizeof(float));
			size+=sizeof(float);
			Buffer.BlockCopy(idByte,0,sPacket,size,idByte.Length);
			return sPacket;
		}
	}
}

