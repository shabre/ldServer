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
        private const float x_under_limit=-150.0f;
        private const float x_upper_limit=150.0f;
        private const float z_under_limit=-150.0f;
        private const float z_upper_limit=150.0f;

        private Position pos;
        private double prevDegree=0;
        public double GetRandomNumber(Random random,double center)
        { 
                double plus=20.0f;
                double minimum=center-plus, maximum=center+plus;
                double val=random.NextDouble() * (maximum - minimum) + minimum;
                if(val>360.0f)
                        return val-360.0f;
                else if(val<0.0f)
                        return val+360.0f;
                else
                        return val;
        }
        public float generatePos(){
                Random random=new Random();
                double mantissa = (random.NextDouble() * 2.0) - 1.0;
                double exponent = Math.Pow(2.0, random.Next(-126, 128));
                return (float)(mantissa * exponent);
        }

        public void NextPosition(){
                float next;
                double degree;
                double angle;
                double vSin, vCos;
                Random random=new Random();
               
                while(true){
                        degree=(float)GetRandomNumber(random,prevDegree);
                        prevDegree=degree;
                        angle=degree*Math.PI/180;

                        vSin=0.15*Math.Sin(angle);
                        vCos=0.15*Math.Cos(angle);
                        if(pos.xPos+vCos<x_upper_limit && pos.xPos+vCos>x_under_limit 
                        && pos.zPos+vSin<z_upper_limit && pos.zPos+vSin>z_under_limit){
                                pos.xPos+=(float)vCos;
                                pos.zPos+=(float)vSin;
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
