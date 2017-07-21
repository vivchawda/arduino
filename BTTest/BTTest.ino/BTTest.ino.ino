// The default baud rate for communication is 9600

#include <SoftwareSerial.h>
SoftwareSerial BTserial(1, 2); // RX | TX

char c = ' ';
int pushButton = 4; //switch is connected to pin4

void setup()
{
  //Serial.begin(9600);
  //Serial.println("Enter AT commands:");
  BTserial.begin(9600);
  pinMode(pushButton, INPUT);
  pinMode(3,OUTPUT); //LED

}

void loop()
{
  int buttonState = digitalRead(pushButton);
  //BTserial.println("Bluetooth Test");
  if (buttonState == 0) {
    BTserial.println("In use");
    //Serial.println("Switch Open");
  }
  else {
    BTserial.println("Resting");
    //Serial.println("Switch closed");
  }
  
//  Serial.println("Bluetooth Test");
//    Serial.println(buttonState);
  
   digitalWrite(3, HIGH);
        
  delay(200);
  
  digitalWrite(3, LOW);
}
