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

// Bluetooth Daten

int motL;
int motR;
uint8_t buf[4];


SoftwareSerial btSerial(4,5);
bool BTconnected;
const byte BTpin = 3;

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

void engine1(int rot) {
  if(rot == 1) {
    digitalWrite(in1, HIGH);  // Motor 1 beginnt zu rotieren
    digitalWrite(in2, LOW);
  } else if(rot == -1) {
    digitalWrite(in1, LOW);  // Motor 1 beginnt rückwärts zu rotieren
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);  // Motor 1 stoppt
    digitalWrite(in2, LOW);
  }
}

void engine2(int rot) {
  if(rot == 1) {
    digitalWrite(in3, HIGH);  // Motor 2 beginnt zu rotieren
    digitalWrite(in4, LOW);
  } else if(rot == -1) {
    digitalWrite(in3, LOW);  // Motor 2 beginnt zu rückwärts rotieren
    digitalWrite(in4, HIGH);
  } else {
    digitalWrite(in3, LOW);  // Motor 2 stoppt
    digitalWrite(in4, LOW);
  }
}

void loop()
{
  if(connectionCheck() && btSerial.available()) //wenn Daten empfangen werden...      
  {
       //Werte zwischen -255 und 255 für die Geschwindigkeit
   btSerial.readBytes(buf, 4);
   motL = buf[1];
   motR = buf[3];
   if(buf[0]) {
     motL = -motL;
   }
   if(buf[2]) {
     motR = -motR;
   }
   //Serial.print("motL = ");
   //Serial.println(motL);
   //Serial.print("motR = ");
   //Serial.println(motR);
   Serial.print(buf[0]);
   Serial.print(",");
   Serial.print(buf[1]);
   Serial.print(",");
   Serial.print(buf[2]);
   Serial.print(",");
   Serial.println(buf[3]);
   Serial.println();
   
   
   if(motL > 35) {
    engine1(1);
   } else if(motL < -35) {
    engine1(-1);
   } else {
    engine1(0);
   }
   if(motR > 35) {
    engine2(1);
   } else if(motR < -35) {
    engine2(-1);
   } else {
    engine2(0);
   }
   analogWrite(GSM1, sqrt(sq(motL)));
   analogWrite(GSM2, sqrt(sq(motR)));
  }
}

bool connectionCheck(){
  while(!BTconnected)
    {
      Serial.print("Robot is looking for the controller\n");
      if (digitalRead(BTpin)==HIGH){
        BTconnected = true;
        Serial.print("Bluetooth connected\n");
        return true;
      } else {
        return false; 
      }
    }
  if(digitalRead(BTpin)==LOW){
    BTconnected = false;
    return false;
  }
  return true;
}
