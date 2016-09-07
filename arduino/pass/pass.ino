#define one 9
#define two 10
#define three 11
#define four 3
//#define in 3
//#define out 4      //tr
int pulsewidth;//定義脈衝寬度
unsigned long f;
int val,val1=150,myangle1;
//下面是servopulse函数部分(此函数意思:也就是說每次都是0.5ms高電平 1.98ms低電平 然後再0.52ms低電平 17ms延時也是低電平)
void servopulse()
{
  	myangle1=map(val1,0,180,500,2480);
  	digitalWrite(one,HIGH);//将舵机接口电平至高
  	digitalWrite(two,HIGH);
  	digitalWrite(three,HIGH);
  	digitalWrite(four,HIGH);
  	delayMicroseconds(myangle1);//延时脉宽值的微秒数
  	digitalWrite(one,LOW);//将舵机接口电平至低
  	digitalWrite(two,LOW);
  	digitalWrite(three,LOW);
  	digitalWrite(four,LOW);
  	delay(20-val1/1000);
}
/*
unsigned long ping(){
	digitalWrite(out,HIGH);
	delayMicroseconds(5);
	digitalWrite(out,LOW);
	return pulseIn(in,HIGH);
}
*/
void setup()
{	
	Serial.begin(9600);
 	pinMode(one,OUTPUT);
 	pinMode(two,OUTPUT);
 	pinMode(three,OUTPUT);
 	pinMode(four,OUTPUT);
 	//pinMode(out,OUTPUT);
	//pinMode(in,INPUT);
  	//看电调说明书，设置油门行程时，一开始需要把遥控器打到最高点。i<=110大概是2杪多
	for(int i=0;i<=110;i++)
		servopulse();
	val1=20;
	//等电机发出B-B两声后（就是两秒后，大概就是这里了）把油门打到最低点
  	for(int i=0;i<=55;i++)
	  servopulse();
	delay(1000);
}
 
void loop()
{
	/*f=ping()/58;
	if(f>50)
		val=3; 
	else
		val=5;*/
		val=3;
	val1=map(val,0,9,0,180);
 	for(int i=0;i<=10;i++)
   		servopulse();
}