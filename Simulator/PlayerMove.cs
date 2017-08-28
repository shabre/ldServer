using System;
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
        public double GetRandomNumber(Random random)
        { 
                double minimum=-1.0f, maximum=1.0f;
                
                return random.NextDouble() * (maximum - minimum) + minimum;
        }
        public float generatePos(){
                Random random=new Random();
                double mantissa = (random.NextDouble() * 2.0) - 1.0;
                double exponent = Math.Pow(2.0, random.Next(-126, 128));
                return (float)(mantissa * exponent);
        }

        public void NextPosition(){
                float next;
                Random xrandom=new Random();
                Random yrandom=new Random();
                while(true){
                        next=(float)GetRandomNumber(xrandom);
                        if(pos.xPos+next<50.0f && pos.xPos+next>-50.0f){
                                pos.xPos+=next;
                                break;
                        }           
                }
                while(true){
                        next=(float)GetRandomNumber(yrandom);
                        if(pos.zPos+next<50.0f && pos.zPos+next>-50.0f){
                                pos.zPos+=next;
                                break;
                        }
                }
        }
        public PlayerMove(){
                pos.xPos=0.0f;
                pos.yPos=2.0f;
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
