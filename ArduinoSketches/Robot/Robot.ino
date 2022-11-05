// Gleichstrommotor 1

const int GSM1 = 6;
const int in1 = 7;
const int in2 = 8;

// Gleichstrommotor 2

const int GSM2 = 11;
const int in3 = 9;
const int in4 = 10;

// Bluetooth Daten
char xSign;
signed char xValue;
char ySign;
signed char yValue;

void setup()
{
  pinMode(GSM1, OUTPUT);    
  pinMode(GSM2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //serieller Monitor wird gestartet, Baudrate auf 9600 festgelegt
  Serial.begin(9600);  
}

enum Rotation {
  forwards, 
  backwards, 
  stopped
};

void engine1(Rotation rot) {
  if(rot == forwards) {
    digitalWrite(in1, HIGH);  // Motor 1 beginnt zu rotieren
    digitalWrite(in2, LOW);
  } else if(rot == backwards) {
    digitalWrite(in1, LOW);  // Motor 1 beginnt rückwärts zu rotieren
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);  // Motor 1 stoppt
    digitalWrite(in2, LOW);
  }
}

void engine2(Rotation rot) {
  if(rot == forwards) {
    digitalWrite(in3, HIGH);  // Motor 2 beginnt zu rotieren
    digitalWrite(in4, LOW);
  } else if(rot == backwards) {
    digitalWrite(in3, LOW);  // Motor 2 beginnt zu rückwärts rotieren
    digitalWrite(in4, HIGH);
  } else {
    digitalWrite(in3, LOW);  // Motor 2 stoppt
    digitalWrite(in4, LOW);
  }
}

void loop()
{
  if(Serial.available()) //wenn Daten empfangen werden...      
  {
   xSign=Serial.read();     //0 für keine Bewegung, 1 für positive Zahlen (vorwärts fahren), -1 für negative Zahlen (rückwärts fahren)
   xValue=Serial.read();    //Werte zwischen 0 und 255 für die Geschwindigkeit
   ySign=Serial.read();
   yValue=Serial.read();

   if(ySign == 1) {//Fall 1: Motor 1 & 2 nach vorne
     engine1(Rotation.forwards);
     engine2(Rotation.forwards);
   } else if(ySign == -1) {
     //Fall 2: Motor 1 & 2 nach hinten
     engine1(Rotation.backwards):
     engine2(Rotation.backwards);
   } else if(ySign == 0 && xSign == 1) {
     //Fall 3: Motor 1 dreht sich nach vorne, Motor 2 dreht sich nach hinten
     engine1(Rotation.forwards);
     engine2(Rotation.backwards);
   } else if(ySign == 0 && xSign == -1) {
     //Fall 4: Motor 1 dreht sich nach hinten, Motor 2 dreht sich nach vorne
     engine1(Rotation.backwards);
     engine2(Rotation.forwards);
   } else {
     //Fall 5: Motor 1 & 2 sind aus
     engine1(Rotation.stopped);
     engine2(Rotation.stopped);
   }
   analogWrite(GSM1, 0);
   analogWrite(GSM2, 0);
  }
}
