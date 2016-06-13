#define one 5
#define two 6
#define three 9
#define four 10 
int i=30;
void setup()
{
	pinMode(one,OUTPUT);
	pinMode(two,OUTPUT);
	pinMode(three,OUTPUT);
	pinMode(four,OUTPUT);
	// analogWrite(one, 200);
	// analogWrite(two, 200);
	// analogWrite(three, 200);
	// analogWrite(four, 200);
	// delay(2000);
	analogWrite(one, 30);
	analogWrite(two, 30);
	analogWrite(three, 30);
	analogWrite(four, 30);
	delay(2000);
}
void loop()
{
	analogWrite(one, i);
	analogWrite(two, i);
	analogWrite(three, i);
	analogWrite(four, i);
	i=i+10;
	if(i==100){
	    i=20;
	}
	delay(500);
}
