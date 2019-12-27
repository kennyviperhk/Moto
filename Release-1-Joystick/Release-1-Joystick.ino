
/*mpu6050*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

#define OUTPUT_READABLE_YAWPITCHROLL


#define INTERRUPT_PIN 7  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

float gyroData[3];

bool isShowPrintln = false;


/*end of mpu6050*/


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


int brake1Range[2] = {477, 486}; //473/474 - 488
int throttle1Range[2] = {180, 850}; //169-850
int brake2Range[2] = {437, 472};//435/474 - 474
int throttle2Range[2] = {192, 850};//170-850

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

#include "EasingLib.h"

Easing easing(EASE_IN_OUT_CUBIC, 10);
Easing easing2(EASE_IN_OUT_CUBIC, 10);


long prevMillis = 0;

void setup() {

  gyroSetup();
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

  if (millis() - prevMillis > 10000) {
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

      Serial.print(   "   A5: ");
      Serial.println(   posVal);
      prevMillis = millis();
    }
  }

  if (pressOnce) {
    pressOnce = false;
    Joystick.releaseButton(currBtn);
  }

  if (pposVal - posVal > abs(10)) {
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
  //Joystick.setXAxis(xAxis);

  Joystick.sendState();
  gyroLoop();
}
