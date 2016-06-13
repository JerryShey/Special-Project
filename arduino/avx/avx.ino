#include <Servo.h>
Servo my[4];
int i=20;
unsigned long f;
char a;
void setup()
{
	// pinMode(7,OUTPUT);
	// pinMode(6,INPUT);
	my[0].attach(9);	//24
	my[1].attach(10);  //13
	my[2].attach(11);
	my[3].attach(3);
	delay(100);
	my[0].writeMicroseconds(1000);
	my[1].writeMicroseconds(1000);
	my[2].writeMicroseconds(1000);
	my[3].writeMicroseconds(1000);
	delay(500);
}
void loop()
{
	// i3=i2=67;
	// i1=30;
	// i4=30;
	// f=ping()/58;
	// if(f<30)
	// 	i=83;
	// else
	// 	i=30;
	// if(a=Serial.read() ){
	// 	if(a=='1')
	// 		i2=i2+1;
	// 	else if(a=='2')
	// 		i1=i1+1;
	// 	else if(a=='3')
	// 		i3=i3+1;
	// 	else if(a=='4')
	// 		i4=i4+1;
	// 	// Serial.print(i2);
	// 	// Serial.print(i1);
	// 	// Serial.print(i3);
	// 	// Serial.println(i4);
	// }
	i=42;
	my[0].write(i);
	my[1].write(i);
	my[2].write(i);
	my[3].write(i);
	delay(15);
}
unsigned long ping(){
	digitalWrite(7,HIGH);
	delayMicroseconds(3);
	digitalWrite(7,LOW);
	return pulseIn(6,HIGH);
}
