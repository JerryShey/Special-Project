#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"

#define in 5
#define out 4      //tr
#define myMax 140
#define myMin 30
#define stop 20

MPU6050 balence;
Servo my[4];
unsigned long f;
int cc,myvalue[4]= {stop, stop ,stop ,stop};
int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup()
{
	Serial.begin(38400);
	balence.initialize();
	pinMode(out,OUTPUT);
	pinMode(in,INPUT);
	my[0].attach(9);	//+x
	my[1].attach(10);  //-x
	my[2].attach(11);	//+y
	my[3].attach(3);	//-y
	// 讓他在低檔位大約 5 秒鐘
	for(cc=0; cc<500; cc++){
		sendSignalToMotor();
		delay(10);
	}
}

void loop()
{
	f=ping()/58;
	if(f>53)	for(cc=0; cc<4; cc++)	myvalue[cc]-=1;
	else 		for(cc=0; cc<4; cc++)	myvalue[cc]+=1;
	int k=10;
	while(k>0){
		balence.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
		if(ax>500)			myvalue[0]-=1;
		else if(ax<-500)	myvalue[1]-=1;
		if(ay>500)			myvalue[2]-=1;
		else if(ay<-500)	myvalue[3]-=1;
		for(cc=0; cc<4; cc++){
		    if(myvalue[cc]>myMax) 		myvalue[cc]=myMax;
		    else if(myvalue[cc]<myMin)	myvalue[cc]=myMin;
		}
		sendSignalToMotor();
		delay(10);	
		k--;
	}
}
//===============================================================================
//===============================================================================

unsigned long ping(){
	digitalWrite(out,HIGH);
	delayMicroseconds(5);
	digitalWrite(out,LOW);
	return pulseIn(in,HIGH);
}

void sendSignalToMotor() {
	for(cc=0; cc<4; cc++)	my[cc].write(myvalue[cc]);
}

/*
void myStop(){
	while(true){
		for(cc=0; cc<4; cc++)	if(myvalue[cc]<stop) goto Iven;
	}
Iven : 
	for(cc=0; cc<4; cc++)	myvalue[cc]=stop;
	sendSignalToMotor();
	while(true){
	    
	}
}
*/