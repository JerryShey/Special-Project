#include <Servo.h>
Servo my[4];
int allvalue=20;
unsigned long f;
int myvalue[4]= {20, 20 ,20 ,20};
void setup()
{

	my[0].attach(9);	//24
	my[1].attach(10);  //13
	my[2].attach(11);
	my[3].attach(3);
	// 讓他在低檔位大約 5 秒鐘
	int d;
	for(d=0; d<500; d++){
		sendSignalToMotor();
		delay(10);
	}
}
void loop()
{
	f=ping()/58;
	if(f>50){
		allvalue=50;
		changevalue(allvalue,allvalue,allvalue,allvalue);
	}
	else{
		allvalue=30;
		changevalue(allvalue,allvalue,allvalue,allvalue);
	}
	sendSignalToMotor();
	delay(10);
}
//===============================================================================
//===============================================================================
unsigned long ping(){
	digitalWrite(7,HIGH);
	delayMicroseconds(5);
	digitalWrite(7,LOW);
	return pulseIn(6,HIGH);
}

void sendSignalToMotor() {
	int c;
	for(c=0; c<4; c++) {
		my[c].write(myvalue[c]);
	}
}

void changevalue(int a1,int a2,int a3,int a4){
	myvalue[0]=a1;
	myvalue[1]=a2;
	myvalue[2]=a3;
	myvalue[3]=a4;
}
