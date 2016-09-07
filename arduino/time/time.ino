#include <Timer.h>

Timer tcb;

void writeToSerial(){
  static  unsigned long count = 0;
  Serial.println(count); // 從0開始輸出，每次加1
  count++;
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  tcb.after(1000, writeToSerial); // 每經過1000毫秒，就會呼叫writeToSerial
}

void loop() {
  
  for(int i=0; i<10000; i++){
      Serial.println(i);
      tcb.update();
  }
  
}