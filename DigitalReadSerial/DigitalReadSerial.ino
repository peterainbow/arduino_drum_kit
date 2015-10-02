/*
  DigitalReadSerial
 Reads a digital input on pin 2, prints the result to the serial monitor

 This example code is in the public domain.
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
int push = 4;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);
  // make the pushbutton's pin an input:
  for ( int pinI = 0; pinI < 6; pinI++ )
    pinMode(pinI, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
//  for ( int pin = 0; pin < 6; pin++ )
  {
  int buttonState = digitalRead(push);
  // print out the state of the button:
//  Serial.print(pin,DEC);
  Serial.println(buttonState,DEC);
  }
  delay(100);        // delay in between reads for stability
}



