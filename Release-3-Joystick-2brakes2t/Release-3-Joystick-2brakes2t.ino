/* Arduino Leonado */

/* Parameters */

float actualInclineAngle = 14; //the angle that the bike and incline
float xAxisDeadZone = 8.0; // angle that wont be triggered

bool isShowPrintln = true;  // Show println result, false on exhibition mode

//int brake1Range[2] = {477, 486}; //473/474 - 488
int brake1Range[2] = {480, 488}; //474/475 - 483  //A0
int throttle1Range[2] = {180, 850}; //169-850
//int brake2Range[2] = {440, 472};//437/474 - 474
int brake2Range[2] = {478, 481};//437/474 - 474  //A3
int throttle2Range[2] = {192, 850};//170-850


/* end of Parameters */
/*
  Imports
*/

#include "Joystick.h"
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

#include <medianFilter.h>

#include "EasingLib.h"

Joystick_ Joystick;
/*mpu6050*/



// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;



// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO


#define LED_PIN 13
bool blinkState = false;


float gyroData[6];
float gyroVal;
float newGyroVal;





medianFilter Filter;

/*end of mpu6050*/



// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
const bool testAutoSendMode = true;

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;





int brake1Pin = A0;    // select the input pin for the potentiometer
int throttle1Pin = A1;    // select the input pin for the potentiometer
int brake2Pin = A3;    // select the input pin for the potentiometer
int throttle2Pin = A4;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int brake1Val = 0;  // variable to store the value coming from the sensor
int throttle1Val = 0;  // variable to store the value coming from the sensor
int brake2Val = 0;  // variable to store the value coming from the sensor
int throttle2Val = 0;  // variable to store the value coming from the sensor

int b1Val = 0;  // variable to store the value coming from the sensor
int newb1Val = 0;  // variable to store the value coming from the sensor
int t1Val = 0;  // variable to store the value coming from the sensor

int b2Val = 0;  // variable to store the value coming from the sensor
int newb2Val = 0;  // variable to store the value coming from the sensor
int t2Val = 0;  // variable to store the value coming from the sensor


int posVal = 0;
int pposVal = 0;
int currBtn = 0;
bool pressOnce = false;


Easing easing(EASE_IN_OUT_CUBIC, 10);
Easing easing2(EASE_IN_OUT_CUBIC, 10);
Easing easing3(EASE_IN_OUT_CUBIC, 10);

long prevMillis = 0;
int brake1ValMin = 0;
int brake2ValMin = 0;
void setup() {

  gyroSetup();
  pinMode(A0, INPUT_PULLUP); //TODO reset accel pos
  pinMode(A3, INPUT_PULLUP); //TODO reset accel pos
  pinMode(13, OUTPUT);

  Serial.begin(115200);
  Serial.println("Calibrating");
  if (millis() < 5000) {
    Serial.print(".");
    int b1 = analogRead(brake1Pin) ;
    if (b1 > brake1ValMin) {
      brake1ValMin = b1;
    }
    int b2 = analogRead(brake2Pin) ;
    if (b2 > brake2ValMin) {
      brake2ValMin = b2;
    }
  }

  //brake1Range[2] = {brake1ValMin, brake1ValMin+8}; //474/475 - 483  //A0
  brake1Range[0] = brake1ValMin + 1;
  brake1Range[1] = brake1ValMin + 4;
  Serial.print("b1 min: ");
  Serial.print(brake1ValMin);
  brake2Range[0] = brake2ValMin + 1;
  brake2Range[1] = brake2ValMin + 4;
  Serial.print(" b2 min: ");
  Serial.print(brake2ValMin);
  Serial.println();
  //Joystick
  Joystick.setXAxisRange(-127, 127);
  Joystick.setThrottleRange(0, 255);
  Joystick.setBrakeRange(0, 255);
  Joystick.setRudderRange(0, 255);
  Joystick.setSteeringRange(0, 300);
  Joystick.setZAxisRange(0, 255);

  Joystick.begin();

  Filter.begin();

}

void loop() {

  // read the value from the sensor:
  brake1Val = analogRead(brake1Pin);
  //  Serial.println(brake1Val);
  throttle1Val = analogRead(throttle1Pin);
  brake2Val = analogRead(brake2Pin);
  throttle2Val = analogRead(throttle2Pin);
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



  if (brake2Val > brake2Range[0]) {
    if (brake2Val > brake2Range[1]) {
      brake2Val = brake2Range[1];
    }
    b2Val = map(brake2Val, brake2Range[0], brake2Range[1], 0, 255);
  } else if (brake2Val <= brake2Range[0])
  {
    b2Val = 0;
  }
  newb2Val = easing2.SetSetpoint(b2Val);

  if (throttle2Val > throttle2Range[0]) {
    if (throttle2Val > throttle2Range[1]) {
      throttle2Val = throttle2Range[1];
    }
    t2Val = map(throttle2Val, throttle2Range[0], throttle2Range[1], 0, 255);
  } else if (throttle2Val <= throttle2Range[0])
  {
    t2Val = 0;
  }
  int g = Filter.run(gyroData[1]);
  gyroVal = map(g, -16000, 16000, -127, 127);
  //newGyroVal = map(gyroVal, -30, 30, -127, 127);
  if (gyroVal > -xAxisDeadZone && gyroVal < 0) {
    gyroVal = 0;
  } else if (gyroVal < xAxisDeadZone && gyroVal >= 0) {
    gyroVal = 0;
  }


  newGyroVal = gyroVal / actualInclineAngle * 90;


  if (newGyroVal > 127) newGyroVal = 127;
  if (newGyroVal < -127) newGyroVal = -127;
  // newGyroVal = easing3.SetSetpoint(gyroVal);

  if (millis() - prevMillis > 500) {
    if (isShowPrintln) {
      Serial.print(   "brake1Val: ");
      Serial.print(   brake1Val);
      Serial.print(   "   throttle1Val: ");
      Serial.print(   throttle1Val);

      Serial.print(   "   newb1Val: ");
      Serial.print(   newb1Val);
      Serial.print(   "   t1Val: ");
      Serial.print(   t1Val);

      Serial.print(   "   brake2Val: ");
      Serial.print(   brake2Val);
      Serial.print(   "   throttle2Val: ");
      Serial.print(   throttle2Val);

      Serial.print(   "   newb2Val: ");
      Serial.print(   newb2Val);
      Serial.print(   "   t2Val: ");
      Serial.print(   t2Val);

      Serial.print(   "   gyroData[0]: ");
      Serial.print(   gyroData[0]);
      Serial.print(   "   gyroData[1]: ");
      Serial.print(   gyroData[1]);
      Serial.print(   "   gyroData[2]: ");
      Serial.print(   gyroData[2]);
      Serial.print(   "   newGyroVal: ");
      Serial.print(   newGyroVal);

      Serial.print(   "   A5: ");
      Serial.println(   newGyroVal);
      prevMillis = millis();
    }
  }

  if (pressOnce) {
    pressOnce = false;
    Joystick.releaseButton(currBtn);
  }

  if (pposVal - posVal > abs(5)) {
    currBtn = map(posVal, 0 , 1024, 0, 32);
    Joystick.pressButton(currBtn);
    pressOnce = true;
  }

  pposVal =  posVal;
  //Joystick
  int finalBVal;
  int finalTVal;
  if (newb1Val > newb2Val) {
    finalBVal = newb1Val;
  } else {
    finalBVal = newb2Val;
  }
  if (t1Val > t2Val) {
    finalTVal = t1Val;
  } else {
    finalTVal = t2Val;
  }
  Joystick.setBrake(finalBVal);
  Joystick.setRudder(finalBVal);
  Joystick.setZAxis(finalBVal);
  Joystick.setThrottle(finalTVal);
  Joystick.setXAxis(newGyroVal);
  Joystick.setYAxis(0);
  Joystick.sendState();
  gyroLoop();
}
