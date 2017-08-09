using System.Collections;
using System.Collections.Generic;


struct Position{
        public float xPos;
        public float yPos;
        public float zPos;
};

//시뮬레이션 플레이어를 움직이도록 만드는 클래스
public class PlayerMove {
  
        private Position pos;
        public PlayerMove(){
                pos.xPos=0.0f;
                pos.yPos=0.0f;
                pos.zPos=0.0f;
        }        
        public float getX(){
                return pos.xPos;
        }
        public float getY(){
                return pos.yPos;
        }
        public float getZ(){
                return pos.zPos;
        }
}
