void setup() {
  //set digital pin 8 as output
  pinMode(13,OUTPUT);
}

void loop(){
  digitalWrite(8,HIGH);//set pin 8 to +5V
  delay(1);//wait 1ms
  digitalWrite(8,LOW);//set pin 8 to 0V
  delay(1);//wait 1ms
}

