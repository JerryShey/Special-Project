#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Timer.h"

#define in 5
#define out 4      //tr
#define myMax 100
#define myMin 30
#define sstop 20

Timer tt;
MPU6050 balence;
Servo my[4];
unsigned long f;
int c,cc,myvalue[4]= {sstop, sstop ,sstop ,sstop};
int16_t ax, ay, az;
int16_t gx, gy, gz;
int lll=0;
void setup()
{
	tt.after(55*1000,myStop);	//55秒後執行myStop()
	Serial.begin(38400);
	balence.initialize();
	delay(10);
	pinMode(out,OUTPUT);
	pinMode(in,INPUT);
	my[0].attach(9);	//+x
	my[1].attach(10);	//-x
	my[2].attach(11);	//+y
	my[3].attach(3);	//-y
	// 讓他在低檔位大約 5 秒鐘
	for(c=0; c<500; c++)	sendSignalToMotor();
}

void loop()
{
	f=ping()/58;	//單位cm
	if(f>53)		down();
	else if(f<50)	up();
	lll++;
	if(lll>1700){
		myStop();	
	}
}
//===============================================================================
//===============================================================================
void down(){				//下降用
	for(c=0; c<4; c++)	myvalue[c]-=1;
	for(c=0; c<3; c++)	check();
}

void up(){					//上升用
	for(c=0; c<4; c++)	myvalue[c]+=1;
	for(c=0; c<3; c++)	check();
}

void turn(){				//旋轉用
	myvalue[0]+=2;
	myvalue[1]+=2;
	for(c=0; c<10; c++)	sendSignalToMotor();
	myvalue[0]-=2;
	myvalue[1]-=2;
	sendSignalToMotor();
}

unsigned long ping(){		//檢查高度
	digitalWrite(out,HIGH);
	delayMicroseconds(5);
	digitalWrite(out,LOW);
	return pulseIn(in,HIGH);
}

void check(){				//檢查是否平衡
	tt.update();			//更新時間
	balence.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	if(ax>-200)			myvalue[1]+=1;
	else if(ax< -700)	myvalue[0]+=1;
	if(ay>500)			myvalue[3]+=1;
	else if(ay< -500)	myvalue[2]+=1;
	for(cc=0; cc<4; cc++){
	    if(myvalue[cc]>myMax) 		myvalue[cc]=myMax;
	    else if(myvalue[cc]<myMin)	myvalue[cc]=myMin;
	}
	sendSignalToMotor();
}

void sendSignalToMotor() {	//使馬達轉
	for(cc=0; cc<4; cc++)	my[cc].write(myvalue[cc]);
	delay(10);
}

void myStop(){
	while(true){
		down();
		for(cc=0; cc<4; cc++)	if(myvalue[cc]<=myMin) goto Iven;
	}
Iven : 
	for(cc=0; cc<4; cc++)	myvalue[cc]=sstop;
	sendSignalToMotor();
	while(true){
	    
	}
}