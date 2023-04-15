#include <ADXL345.h>
#include <Wire.h>
#include <SoftwareSerial.h>

//Generelles
int junk;
int motRInt;
int motLInt;
uint8_t buf[4];
enum controlStyle {JOYSTICK, TILT_SENSOR};
controlStyle currentStyle = JOYSTICK;
ADXL345 adxl;

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

//Joystick
const int SW = 2;
const int VRx = 7;
const int VRy = 6;
bool buttonPressed;

void setup() {
  pinMode(SW, INPUT_PULLUP);
  btSerial.begin(38400);
  Serial.begin(38400);
  adxl.powerOn();
}

bool yAchsePos() {
  if (yAchse > Fix + marche) return true;
  else return false;
}

bool yAchseNeg() {
  if (yAchse < Fix - marche) return true;
  else return false;
}

bool xAchsePos() {
  if (xAchse > Fix + marche) return true;
  else return false;
}

bool xAchseNeg() {
  if (xAchse < Fix - marche) return true;
  else return false;
}

void calculateSpeed() {
  Speed = sqrt(sq(xAchse - haelfte) + sq(yAchse - haelfte));
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
      Speed = sqrt(sq(yAchse - haelfte));
    }
  }
  else {
    if (xAchsePos()) {
      motL = 1;
      motR = -1;
      Speed = xAchse - haelfte;
    }
    else if (xAchseNeg()) {
      motL = -1;
      motR = 1;
      Speed = sqrt(sq(xAchse - haelfte));
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
      if (digitalRead(BTpin)==HIGH){
        BTconnected = true;
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

void readData() {
  buttonSwitch();
  if(currentStyle == JOYSTICK) {
      xAchse = analogRead(VRx);
    } else {
      adxl.readXYZ(&xAchse, &junk, &junk);
      xAchse = int(map(xAchse, -256, 255, 0, 1023));
    }
    yAchse = analogRead(VRy);

    if (xAchse < 576 && xAchse > 448) xAchse = 512;
    if (yAchse < 576 && yAchse > 448) yAchse = 512;
    if (yAchse > 900) xAchse = 512;
    if (yAchse < 128) xAchse = 512;
}

void loop() {  
  if(connectionCheck()){
    readData();
    calculateSpeed();
    bluetoothTransmission();
  }
  delay(100);
}
