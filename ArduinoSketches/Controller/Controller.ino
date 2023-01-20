#include <ADXL345.h>
#include <Wire.h>
#include <SoftwareSerial.h>

//Generelles
int junk;
int motRInt;
int motLInt;
uint8_t buf[4];
unsigned long timer = 500;
unsigned long previousMillis = 0;

enum controlStyle {JOYSTICK, TILT_SENSOR};
controlStyle currentStyle = JOYSTICK;

//Bluetooth Kommunikation
SoftwareSerial btSerial(5,4);
bool BTconnected;
const byte BTpin = 3;

//Speed Calculation
int xAchse;
int yAchse;
float Speed;
float Fix = 512;
float haelfte = 512;
float alpha;
float motR = 0;
float motL = 0;
int marche = 64;

//ControllerIO
//const int ledPin;
bool buttonPressed;

//Joystick

const int SW = 2;
const int VRx = 7;
const int VRy = 6;

//Neigungssensor
ADXL345 adxl;

//Setup-Funktion
void setup() {
  pinMode(SW, INPUT_PULLUP);
  //pinMode(ledPin, OUTPUT);
  btSerial.begin(38400);
  Serial.begin(38400);
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

bool yAchsePos() {
  if (yAchse > Fix+marche) return true;
  else return false;
}

bool yAchseNeg() {
  if (yAchse < Fix-marche) return true;
  else return false;
}

bool xAchsePos() {
  if (xAchse > Fix+marche) return true;
  else return false;
}

bool xAchseNeg() {
  if (xAchse < Fix-marche) return true;
  else return false;
}

//Funktion zur Berechnung der Joystick-Inputdaten und Schreiben der Daten in die Bluetooth Kommunikation
//
void calculateSpeed() {
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

  
}

bool connectionCheck(){
  while(!BTconnected)
    {
      Serial.print("Controller is looking for the robot\n");
      if (digitalRead(BTpin)==HIGH){
        BTconnected = true;
        Serial.print("Bluetooth connected\n");
        return true;
      } 
      else{
        return false;
      }   
  if(digitalRead(BTpin)==LOW){
    BTconnected = false;
    return false;
  }
  return true;
  }
}

void bluetoothTransmission(){
  if(motLInt < -64) {
    buf[0] = 0;
    buf[1] = static_cast<uint8_t>(-motLInt);
  } else if (motLInt > 64) {
    buf[0] = 2;
    buf[1] = static_cast<uint8_t>(motLInt);
  } else {
    buf[0] = 1;
    buf[1] = 0;
  }

  if(motRInt < -64) {
    buf[2] = 0;
    buf[3] = static_cast<uint8_t>(-motRInt);
  } else if(motRInt > 64) {
    buf[2] = 2;
    buf[3] = static_cast<uint8_t>(motRInt);
  } else {
    buf[2] = 1;
    buf[3] = 0;
  }
  btSerial.write(buf, 4);
}

void buttonSwitch() {
  if(buttonPressed && digitalRead(SW) == LOW) {
    if(currentStyle == JOYSTICK) {
      currentStyle = TILT_SENSOR;
    } else {
      currentStyle = JOYSTICK;
    }
    buttonPressed = false;
  }
 
  buttonPressed = digitalRead(SW);
}

void debug() {
  Serial.print("Current Style = ");
  Serial.println(currentStyle);
  Serial.print(buf[0]-1);
  Serial.print(",");
  Serial.print(buf[1]);
  Serial.print(",");
  Serial.print(buf[2]-1);
  Serial.print(",");
  Serial.println(buf[3]);
  Serial.println();
  Serial.print("motLInt = ");
  Serial.println(motLInt);
  Serial.print("motRInt = ");
  Serial.println(motRInt);
  Serial.println();
  Serial.print("xAchse = ");
  Serial.println(xAchse - 512);
  Serial.print("yAchse = ");
  Serial.println(yAchse - 512);
  Serial.println();
}

//Loop-Funktion
void loop() {  
    buttonSwitch();

    if(currentStyle == JOYSTICK) {
      xAchse = analogRead(VRx);
    } else {
      adxl.readXYZ(&xAchse, &junk, &junk);
      xAchse -= 30;
      xAchse = int(map(xAchse, -255, 255, 0, 1024));
    }
    yAchse = analogRead(VRy) + 17;

    //korrekturen
    if (xAchse < 576 && xAchse > 448) xAchse = 512;
    if (yAchse < 576 && yAchse > 448) yAchse = 512;
    if (yAchse > 900) xAchse = 512;
    if (yAchse < 128) xAchse = 512;


    if(connectionCheck()){
      calculateSpeed();
      bluetoothTransmission();
      debug();
    }
  delay(100);
  
}
