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

	my[0].attach(9);	//+x
	my[1].attach(10);  //-x
	my[2].attach(11);	//+y
	my[3].attach(3);	//-y
	// 讓他在低檔位大約 5 秒鐘
	for(cc=0; cc<500; cc++){
		sendSignalToMotor();
		delay(10);
	}
	Serial.begin(38400);
	Serial.println("hihi");
	myvalue[0]=30;
	myvalue[1]=30;
	myvalue[2]=30;
	myvalue[3]=30;
	pinMode(13,OUTPUT);
	digitalWrite(13, LOW);
}

void loop()
{
	sendSignalToMotor();
	delay(30);
}
//===============================================================================
//===============================================================================

void sendSignalToMotor() {
	for(cc=0; cc<4; cc++)	my[cc].write(myvalue[cc]);
}
