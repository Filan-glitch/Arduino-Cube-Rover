#include <SoftwareSerial.h>
#include <string.h>

// Gleichstrommotor 1
const int GSM1 = 6;
const int in1 = 7;
const int in2 = 8;

// Gleichstrommotor 2
const int GSM2 = 11;
const int in3 = 9;
const int in4 = 10;

enum Rotation {FORWARDS = 1, BACKWARDS = -1, STANDSTILL = 0};

// Bluetooth Daten
uint8_t buf[4];
SoftwareSerial btSerial(3,4);
bool BTconnected;
const byte BTpin = 5;

void setup()
{
  pinMode(GSM1, OUTPUT);    
  pinMode(GSM2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  btSerial.begin(38400);
  Serial.begin(38400);

}

void loop()
{
  if(connectionCheck() && btSerial.available())    
  {
   btSerial.readBytes(buf, 4);

   engine1(buf[2]-1);
   engine2(buf[0]-1);

   analogWrite(GSM1, buf[3]);
   analogWrite(GSM2, buf[1]);
  }
}

bool connectionCheck(){
  while(!BTconnected)
    {
      if (digitalRead(BTpin)==HIGH){
        BTconnected = true;
        return true;
      }
    }
    
  if(digitalRead(BTpin)==LOW){
    engine1(0);
    engine2(0);
    analogWrite(GSM1, 0);
    analogWrite(GSM2, 0);
    BTconnected = false;
    return false;
  }
  return true;
}

void engine1(Rotation rot) {
  if(rot == FORWARDS) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if(rot == BACKWARDS) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}

void engine2(Rotation rot) {
  if(rot == FORWARDS) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else if(rot == BACKWARDS) {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}
