#include <SoftwareSerial.h>
#include <string.h>
//#include <MFRC522.h>
//#include <SPI.h>

// Gleichstrommotor 1

const int GSM1 = 6;
const int in1 = 7;
const int in2 = 8;

// Gleichstrommotor 2

const int GSM2 = 11;
const int in3 = 9;
const int in4 = 10;

// Bluetooth Daten

uint8_t motL;
uint8_t motR;
uint8_t buf[4];


SoftwareSerial btSerial(4,5);
bool BTconnected;
const byte BTpin = 3;

//NFC Daten
//const int SS_PIN = ; //muss noch aktualisiert werden
//const int RST_PIN = ; //muss noch aktualisiert werden
//MFRC522 mfrc522(SS_PIN, RST_PIN);
//long forward = 0; //rfid anpassen
//long backward = 0; //...
//long right = 0; //...
//long left = 0; //...



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
  //SPI.begin(); // SPI-Verbindung aufbauen
  //mfrc522.PCD_Init(); // Initialisierung des RFID-Empfängers
}

void loop()
{
  //nfcReader();
  if(connectionCheck() && btSerial.available()) //wenn Daten empfangen werden...      
  {
       //Werte zwischen -255 und 255 für die Geschwindigkeit
   btSerial.readBytes(buf, 4);

   motL = buf[1];
   motR = buf[3];

   engine1(buf[2]-1);
   engine2(buf[0]-1);

   analogWrite(GSM1, motR);
   analogWrite(GSM2, motL);

   debug();
  }
}
/*
void rfidReader() {
  long code=0;
  // Überprüfen, ob ein NFC-Tag gefunden wurde
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Den Inhalt des NFC-Tags auslesen
    uint8_t tagData[16];
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      code=((code+mfrc522.uid.uidByte[i])*10);
    }

    // Die Funktion nfcMove aufrufen, um den Motor anzusteuern
    nfcMove(code);
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) // Wenn keine Karte in Reichweite ist...
  {
    return; // ...springt das Programm zurück vor die if-Schleife, womit sich die Abfrage wiederholt.
  }
}

void nfcMove(long code) {
  // Überprüfen, welcher Inhalt im NFC-Tag gespeichert ist
  switch (code) {
    case forward:
       // Den Motor nach vorne bewegen
      engine1(1);
      engine2(1);
      analogWrite(GSM1, 255);
      analogWrite(GSM2, 255);
      delay(3000);
      analogWrite(GSM1, 0);
      analogWrite(GSM2, 0);
      engine1(0);
      engine2(0);
      break;
    case backward:
      // Den Motor nach hinten bewegen
      engine1(-1);
      engine2(-1);
      analogWrite(GSM1, 255);
      analogWrite(GSM2, 255);
      delay(3000);
      analogWrite(GSM1, 0);
      analogWrite(GSM2, 0);
      engine1(0);
      engine2(0);
      break;
    case left:
      // Den Motor nach links drehen
      engine1(-1);
      engine2(1);
      analogWrite(GSM1, 255);
      analogWrite(GSM2, 255);
      delay(3000);
      analogWrite(GSM1, 0);
      analogWrite(GSM2, 0);
      engine1(0);
      engine2(0);
      break;
    case right:
      // Den Motor nach rechts drehen
      engine1(1);
      engine2(-1);
      analogWrite(GSM1, 255);
      analogWrite(GSM2, 255);
      delay(3000);
      analogWrite(GSM1, 0);
      analogWrite(GSM2, 0);
      engine1(0);
      engine2(0);
      break;
    default:
      // Keine Aktion ausführen, wenn kein unterstützter Befehl gefunden wurde
      break;
  }
}
*/
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

void debug() {
  //Serial.print("motL = ");
  //Serial.println(motL);
  //Serial.print("motR = ");
  //Serial.println(motR);
  Serial.print(buf[0]-1);
  Serial.print(",");
  Serial.print(buf[1]);
  Serial.print(",");
  Serial.print(buf[2]-1);
  Serial.print(",");
  Serial.println(buf[3]);
  Serial.println();
}
