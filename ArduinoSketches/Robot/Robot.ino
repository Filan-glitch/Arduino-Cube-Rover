// Gleichstrommotor 1

const int GSM1 = 6;
const int in1 = 7;
const int in2 = 8;

// Gleichstrommotor 2

const int GSM2 = 11;
const int in3 = 9;
const int in4 = 10;

// Bluetooth Daten
int xValue;
int yValue;

//Test Daten
int motor1;


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


void loop()
{
  if(Serial.available()) //wenn Daten empfangen werden...      
  {
   
   xValue=Serial.read();    //Werte zwischen -255 und 255 für die Geschwindigkeit
   yValue=Serial.read();
   
   
   if(yValue > 25) {//Fall 1: Motor 1 & 2 nach vorne
     engine1(forwards);
     engine2(forwards);
   } else if(yValue < -25) {
     //Fall 2: Motor 1 & 2 nach hinten
     engine1(backwards):
     engine2(backwards);
   } else if(25 >= yValue >= -25 && xValue > 25) {
     //Fall 3: Motor 1 dreht sich nach vorne, Motor 2 dreht sich nach hinten
     engine1(forwards);
     engine2(backwards);
   } else if(25 >= yValue >= -25 && xValue < 25) {
     //Fall 4: Motor 1 dreht sich nach hinten, Motor 2 dreht sich nach vorne
     engine1(backwards);
     engine2(forwards);
   } else {
     //Fall 5: Motor 1 & 2 sind aus
     engine1(stopped);
     engine2(stopped);
   }
   


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
