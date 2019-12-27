
#include "Joystick.h"
Joystick_ Joystick;

// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
//const bool testAutoSendMode = true;
const bool testAutoSendMode = true;

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;


int brake1Range[2] = {480, 488};
int throttle1Range[2] = {192, 850};

int brake1Pin = A0;    // select the input pin for the potentiometer
int throttle1Pin = A1;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int brake1Val = 0;  // variable to store the value coming from the sensor
int throttle1Val = 0;  // variable to store the value coming from the sensor

int b1Val = 0;  // variable to store the value coming from the sensor
int newb1Val = 0;  // variable to store the value coming from the sensor
int t1Val = 0;  // variable to store the value coming from the sensor


int posVal = 0;
int pposVal = 0;
int currBtn = 0;
bool pressOnce = false;

#include "EasingLib.h"

Easing easing(EASE_IN_OUT_CUBIC, 10);

void setup() {


  pinMode(A0, INPUT_PULLUP); //TODO reset accel pos
  pinMode(13, OUTPUT);

  Serial.begin(115200);

  //Joystick
  Joystick.setXAxisRange(-127, 127);
  Joystick.setThrottleRange(0, 255);
  Joystick.setBrakeRange(0, 255);
  Joystick.setRudderRange(0, 255);
  Joystick.setSteeringRange(0, 300);
  Joystick.setZAxisRange(0, 255);

  Joystick.begin();

}

void loop() {

  // read the value from the sensor:
  brake1Val = analogRead(brake1Pin);
  throttle1Val = analogRead(throttle1Pin);
  posVal = analogRead(A5);
  /*
    b1Val = map(brake1Val, brake1Range[0], brake1Range[1], 0, 255);
    if (brake1Val > brake1Range[0]) {

      b1Val = 0;

    } else if (brake1Val <= brake1Range[1])
    {
      b1Val = 255;
    }

    newb1Val = easing.SetSetpoint(b1Val);
  */


  if (brake1Val > brake1Range[0]) {
    if (brake1Val > brake1Range[1]) {
      brake1Val = brake1Range[1];
    }
    b1Val = map(brake1Val, brake1Range[0], brake1Range[1], 0, 255);
  } else if (brake1Val <= brake1Range[0])
  {
    b1Val = 0;
  }
  newb1Val = easing.SetSetpoint(b1Val);

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
  Serial.print(   newb1Val);
  Serial.print(   "   A1: ");
  Serial.print(   t1Val);

  Serial.print(   "   A5: ");
  Serial.println(   posVal);

  if (pressOnce) {
    pressOnce = false;
    Joystick.releaseButton(currBtn);
  }

  if (pposVal- posVal > abs(10)) {
    currBtn = random(0, 10);
    Joystick.pressButton(currBtn);
    pressOnce = true;
  }

  pposVal =  posVal;
  //Joystick

  Joystick.setBrake(newb1Val);
  Joystick.setRudder(newb1Val);
  Joystick.setZAxis(newb1Val);
  Joystick.setThrottle(t1Val);

  Joystick.sendState();

}
