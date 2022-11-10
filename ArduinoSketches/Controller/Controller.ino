#include <ADXL345.h>
#include <Wire.h>

//Generelles
int x,y,z;
int motRInt;
int motLInt; 
enum controlStyle {JOYSTICK, TILT_SENSOR};
controlStyle currentStyle = JOYSTICK;

//Speed Calculation
int xAchse;
int yAchse;
float Speed;
float Fix = 519;
float haelfte = 512;
float alpha;
float motR = 0;
float motL = 0;
int marche = 8;

//ControllerIO
const int ledPin;
const int buttonPin;
bool buttonPressed;

//Joystick

const int SW = 32;
const int VRx = 35;
const int VRy = 34;

//Neigungssensor
const int IN2 = 33;
ADXL345 adxl;

//Setup-Funktion
void setup() {
  pinMode(SW, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
}

//Loop-Funktion
void loop() {
  if(buttonPressed && !digitalRead(buttonPin)) {
    if(currentStyle == JOYSTICK) {
      currentStyle == TILT_SENSOR;
    } else {
      currentStyle == JOYSTICK;
    }
    buttonPressed = false;
  }
 
  buttonPressed = digitalRead(buttonPin);
  
  if(currentStyle = JOYSTICK) {
    xAchse = analogRead(VRx);
    yAchse = analogRead(VRy);
  } else {
    adxl.readXYZ(&x, &y, &z);
  }
  calculateSpeed(x,y);
}

boolean yAchsePos() {
  if (yAchse > Fix+marche) return true;
  else return false;
}

boolean yAchseNeg() {
  if (yAchse < Fix-marche) return true;
  else return false;
}

boolean xAchsePos() {
  if (xAchse > Fix+marche) return true;
  else return false;
}

boolean xAchseNeg() {
  if (xAchse < Fix-marche) return true;
  else return false;
}

//Funktion zur Berechnung der Joystick-Inputdaten und Schreiben der Daten in die Bluetooth Kommunikation
void calculateSpeed(int xValue, int yValue) {
  Speed = sqrt(sq(xAchse-haelfte)+sq(yAchse-haelfte));
  if (yAchsePos()) {
    if (xAchsePos()) {
      motR = -1;
      alpha = degrees(asin(map(yAchse, haelfte, 1023, 0, 1)));
      motL = map(alpha, 0, 90, 1, -1);
    }
    else if(xAchseNeg()) {
      motL = -1;
      alpha = degrees(asin(map(yAchse, haelfte, 1023, 0, 1)));
      motR = map(alpha, 0, 90, 1, -1);
    }
    else {
      motR = -1;
      motL = -1;
      Speed = yAchse-haelfte;
    }
  }
  else if (yAchseNeg()) {
    if (xAchsePos()) {
      motL = 1;
      alpha = degrees(asin(map(yAchse, haelfte, 0, 0, 1)));
      motR = map(alpha, 0, 90, -1, 1);
    }
    else if (xAchseNeg()) {
      motR = 1;
      alpha = degrees(asin(map(yAchse, haelfte, 0, 0, 1)));
      motL = map(alpha, 0, 90, -1, 1);
    }
    else {
      motR = 1;
      motL = 1;
      Speed = sqrt(sq(yAchse-haelfte));
    }
  }
  else {
    if (xAchsePos()) {
      motL = 1;
      motR = -1;
      Speed = xAchse-haelfte;
    }
    else if (xAchseNeg()) {
      motL = -1;
      motR = 1;
      Speed = sqrt(sq(xAchse-haelfte));
    }
    else {
      motL = 0;
      motR = 0;
      Speed = 0;
    }
  }
  if (Speed > 512) Speed = 512;
  motR *= Speed;
  motL *= Speed;
  motLInt = int(map(motL, -512, 512, -255, 255));
  motRInt = int(map(motR, -512, 512, -255, 255));
  Serial.write(motLInt);
  Serial.write(motRInt);
  
}
