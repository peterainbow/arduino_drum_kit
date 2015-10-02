/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

 This example code is in the public domain.
 */

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);                                  // connect to the serial port 115200
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A1);
  unsigned long t = micros();
  // print out the value you read:
  if (sensorValue > 1)
  {
    Serial.print("t: ");
    Serial.print(t,DEC);
    Serial.print(" v: ");
    Serial.println(sensorValue,DEC);
  }
  delay(1);        // delay in between reads for stability
}
