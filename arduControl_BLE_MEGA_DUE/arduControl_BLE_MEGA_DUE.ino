// idea's Dev.
// www.ideascp.es
// iOS, Android, Electronics
// Control Pins Arduino by HM-10 Bluetooth LE
// Arduino MEGA - DUE or compatible

// Configure Pins
int pinToAttach;
int pinValue;

// Total number of pins to control + the last element '0'
#define NUMBER_OF_PINS 11

// Digital pins to control
// The end of the array is determinated by the
// last element '0' 
// If you want to control the pins from 2 to 11
// your array will be same as:
int arrayPins[NUMBER_OF_PINS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0};

// String communications
String tmp;

void setup()  {
  
  // Setup communication
  Serial.begin(57600); // Serial USB Arduino
  
  // Serial2 is BLE HM-10
  // Tx from HM-10 to pin 17 Arduino
  // Rx from HM-10 to pin 16 Arduino
  // GND to GND
  // Vin from HM-10 to 3,3 V Arduino
  Serial2.begin(57600); // Serial2 17 & 16 Arduino MEGA
}

void loop ()  {

  // BLE communication
  while (Serial2.available() > 0)  {
  tmp += char(Serial2.read());
  delay(2);  
  }
  
  // BLE Reception
  if(tmp.length() > 0)  {
    
    String bufferString = tmp;
    
    // Android question...
    if (tmp[0] == '?' && tmp[1] == 'p' && tmp[2] == 'i' && tmp[3] == 'n' && tmp[4] == 's')  {
      
      // Arduino answer...
        for (int i = 0; i < NUMBER_OF_PINS; i++)  {
           Serial2.println(arrayPins[i]);
           delay(30);
           }
    }
    
    // Pin Control
    // Received string example: Pin 021
    // Pin 021 -> Pin 2 ON ; Pin 020 -> Pin 2 OFF
    if(tmp[0] == 'P' && tmp[1] == 'i' && tmp[2] == 'n')  {
      
      String stringPin = bufferString.substring(4,6);
      pinToAttach = stringPin.toInt();
      
      String stringPinValue = bufferString.substring(6,7);
      pinValue = stringPinValue.toInt();

      pinMode(pinToAttach, OUTPUT);
      digitalWrite(pinToAttach, pinValue);
      
    } // End if(tmp[0] == 'P' && tmp[1] == 'i' && tmp[2] == 'n')
    
    // Print received String...
    // Serial.println(tmp);
    
    // Clear received String
    tmp = "";
    
  } // End "if(tmp.length() > 0)"
  
  // Send String from Arduino Serial
  // to BLE Serial
  // if (Serial.available()) {
  //   Serial2.write(Serial.read());
  // }
}

// Send data to App. Function
void sendStringTo(String msg)  {

  Serial2.println(msg);
}
