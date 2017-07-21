const byte LED = 10;
const byte Button = 2;

byte ButtonState;
byte lastState = LOW;
byte count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(Button, INPUT);
}

void loop() {
  ButtonState = digitalRead(Button);

 if(ButtonState && ButtonState != lastState)  // button latch, no debounce needed.
  //  if(ButtonState = HIGH)  
  {
    if(count < 3) // This will check to see if the count is within a range of 0 - 255, and anything over that, it will reset count back to 0. Of course, this will happen anyways because count is a BYTE, and not an int or any other type.
      count += 1; // same as count = count + 5;
    else
      count = 0;
      
    //analogWrite(LED, count);
    switch (count) {
       case 0:
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
        break;
        case 2:
        digitalWrite(10, LOW);
        digitalWrite(11, HIGH);
        digitalWrite(12, LOW);
        break; 
        case 3:
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
        digitalWrite(12, HIGH);
        break;
        case 1:
        digitalWrite(10, HIGH);
        digitalWrite(11, LOW);
        digitalWrite(12, LOW);
        break;   
    }
    Serial.println(count);
  } 
  lastState = ButtonState;
delay(200);
}

