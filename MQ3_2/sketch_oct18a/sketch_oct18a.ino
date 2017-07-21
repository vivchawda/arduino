#include <Wire.h>
#include <Adafruit_MLX90614.h>
int sensorValue;
int pin8 = 8;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup()
{
	Serial.begin(9600);      // sets the serial port to 9600
	pinMode(pin8, OUTPUT);
	Serial.println("Adafruit MLX90614 test");
    mlx.begin();
}

void loop()
{
	sensorValue = analogRead(0);       // read analog input pin 0
	//Serial.print(sensorValue, DEC);  // prints the value read
	//Serial.println("ppm");
	Serial.println(50);
        if (sensorValue > 500) {
		// Activate digital output pin 8 - the LED will light up
		digitalWrite(pin8, HIGH);
	}
	else {
		// Deactivate digital output pin 8 - the LED will not light up
		digitalWrite(pin8, LOW);
	}

// Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
// Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
 //Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF());
// Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

// Serial.println();	
	delay(1000);                        // wait 100ms for next reading
}
