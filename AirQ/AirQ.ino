// GAS SENSOR MQ-2

// This sensor can detect smoke, methane, carbon dioxide and other gases


//VARIABLES
float Ro = 10000.0;    // this has to be tuned 10K Ohm
int sensorPin = 0;  // select the input pin for the sensor
int ledPin = 13;    // select the pin for the LED
int val = 0;        // variable to store the value coming from the sensor
float Vrl = 0.0;
float Rs = 0.0;
float ratio = 0.0;



// SETUP
void setup() {
pinMode(ledPin, OUTPUT); // declare the ledPin as an OUTPUT
Serial.begin(9600);      // initialize serial communication with computer
// analogReference(EXTERNAL);
}

// get CO ppm
float get_CO (float ratio){
  float ppm = 0.0;
  ppm = 37143 * pow (ratio, -3.178);
return ppm;
}

// LOOP
void loop() { 
  
val = analogRead(sensorPin);     // read the value from the analog sensor
Serial.println(val);             // send it to the computer (as ASCII digits)
digitalWrite(ledPin, HIGH);      // turn the ledPin on
delay(100);                      // stop the program for some time
digitalWrite(ledPin, LOW);       // turn the ledPin off
delay(100);                      // stop the program for some time

Vrl = val * ( 5.00 / 1024.0  );      // V
Rs = 20000 * ( 5.00 - Vrl) / Vrl ;   // Ohm 
ratio =  Rs/Ro;                      

Serial.print ( "Vrl / Rs / ratio:");
Serial.print (Vrl);
Serial.print(" ");
Serial.print (Rs);
Serial.print(" ");
Serial.println(ratio);
Serial.print ( "CO ppm :");
Serial.println(get_CO(ratio));
Serial.println("");


delay(2000);
}

