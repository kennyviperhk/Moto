/*
  Analog Input
  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED)  connected to digital pin 13.
  The amount of time the LED will be on and off depends on
  the value obtained by analogRead().

  The circuit:
   Potentiometer attached to analog input 0
   center pin of the potentiometer to the analog pin
   one side pin (either one) to ground
   the other side pin to +5V
   LED anode (long leg) attached to digital output 13
   LED cathode (short leg) attached to ground

   Note: because most Arduinos have a built-in LED attached
  to pin 13 on the board, the LED is optional.


  Created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://arduino.cc/en/Tutorial/AnalogInput

*/
int brake1Range[2] = {513, 507};
int throttle1Range[2] = {190, 860};

int brake1Pin = A0;    // select the input pin for the potentiometer
int throttle1Pin = A1;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int brake1Val = 0;  // variable to store the value coming from the sensor
int throttle1Val = 0;  // variable to store the value coming from the sensor

int b1Val = 0;  // variable to store the value coming from the sensor
int t1Val = 0;  // variable to store the value coming from the sensor
void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // read the value from the sensor:
  brake1Val = analogRead(brake1Pin);
  throttle1Val = analogRead(throttle1Pin);

  b1Val = map(brake1Val, brake1Range[0], brake1Range[1], 0, 255);
  if (brake1Val > brake1Range[0]) {

    b1Val = 0;

  } else if (brake1Val <= brake1Range[1])
  {
    b1Val = 255;
  }


  if (throttle1Val > throttle1Range[0]) {
    if (throttle1Val > throttle1Range[1]) {
      throttle1Val = throttle1Range[1];
    }
    t1Val = map(throttle1Val, throttle1Range[0], throttle1Range[1], 0, 255);
  } else if (throttle1Val <= throttle1Range[0])
  {
    t1Val = 0;
  }
  Serial.print(   "A0: ");
  Serial.print(   brake1Val);
  Serial.print(   "   A1: ");
  Serial.print(   throttle1Val);
  
  Serial.print(   "   A0: ");
  Serial.print(   b1Val);
  Serial.print(   "   A1: ");
  Serial.println(   t1Val);
}
