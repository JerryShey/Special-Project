#include <Stepper.h>
#define STEPS 2048

Stepper stepper(STEPS, 11, 9, 10, 8);

void setup() {
	stepper.setSpeed(5);
	Serial.begin(9600);
}

void loop() {
	if(Serial.available()){
		char a=Serial.read();
		if(a=='1'){
			stepper.step(25);
		    delay(200);
		}
		else if(a=='2'){
			stepper.step(-25);
       		delay(200);
		}
		else{

		}
	}
}
/*
void anticlockwise()		//逆時鐘
{
	for(int i = 0; i < 8; i++)
	{
		setOutput(i);
		delayMicroseconds(motorSpeed);
	}
}

void clockwise()			//順時鐘
{
	for(int i = 7; i >= 0; i--)
	{
		setOutput(i);
		delayMicroseconds(motorSpeed);
	}
}
void setOutput(int out)
{
	digitalWrite(motorPin1, bitRead(lookup[out], 0));
	digitalWrite(motorPin2, bitRead(lookup[out], 1));
	digitalWrite(motorPin3, bitRead(lookup[out], 2));
	digitalWrite(motorPin4, bitRead(lookup[out], 3));
}
void stop(){
	digitalWrite(motorPin1, 0);
	digitalWrite(motorPin2, 0);
	digitalWrite(motorPin3, 0);
	digitalWrite(motorPin4, 0);	
}*/
