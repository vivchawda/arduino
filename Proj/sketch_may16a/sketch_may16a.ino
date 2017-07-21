/*
 * Sketch1.ino
 *
 * Created: 5/16/2015 4:58:34 PM
 * Author: VivC
 */ 
int val = 0;
void setup()
{
Serial.begin(9600);		
	  /* add setup code here, setup code runs once when the processor starts */
}

void loop()
{
	
val = analogRead(0);
Serial.println(val);
	  /* add main program code here, this code starts again each time it ends */

}

