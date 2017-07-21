
extern unsigned long timer0_overflow_count;
 
int clockA = 2;          // Clock wires
int clockB = 3;          // to.  Order is not important.
 
int tickPin = clockA;    // This keeps track of which clock pin should be fired next.
 
 
// Initialize the IO ports
void setup()
{
  pinMode(clockA, OUTPUT);
  pinMode(clockB, OUTPUT);
 
  digitalWrite(clockA, LOW);
  digitalWrite(clockB, LOW);
 
  Serial.begin(9600);
}
 
 
// Move the second hand forward one position (one second on the clock face).
void doTick() {
 
  // Energize the electromagnet in the correct direction.
  digitalWrite(tickPin, HIGH);
  delay(10);
  digitalWrite(tickPin, LOW);  
 
  // Switch the direction so it will fire in the opposite way next time.
  if (tickPin == clockA)
  {
    tickPin = clockB;
  } else {
    tickPin = clockA;
  }
}
 
 
// Main loop
void loop()
{
  unsigned long startTime = millis();
  unsigned long temp;
 
  // Pretend to be a regular clock, and tick once a second.
  while (true)
  {
    startTime += 1000;
 
    // Wait until a second has passed.  Note that this will do ugly things when millis()
    // runs over, so we only have about 9 hours before this version will stop working.
    while (startTime - millis() > 0) {}
 
    doTick();
  }
}
