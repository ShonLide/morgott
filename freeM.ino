#include <Servo.h>

//I/O
int En = 2;
int IBelieve = 3;
int Thrones = 4;
int Hands = 5;
int Erdtree = 6;
int Leash = 7;
int YouToo = 8;

//Servo
Servo myservo;
int pos = 30;

//voice
int voice;

void setup() {
  // put your setup code here, to run once:
  myservo.attach(9, 500, 2500);

  Serial.begin(9600);

  pinMode(En, OUTPUT);  
  pinMode(IBelieve, OUTPUT);  
  pinMode(Thrones, OUTPUT);  
  pinMode(Hands, OUTPUT);  
  pinMode(Erdtree, OUTPUT);  
  pinMode(Leash, OUTPUT);  
  pinMode(YouToo, OUTPUT);   
  digitalWrite(En, HIGH); 
  digitalWrite(IBelieve, HIGH); 
  digitalWrite(Thrones, HIGH); 
  digitalWrite(Hands, HIGH); 
  digitalWrite(Erdtree, HIGH);
  digitalWrite(Leash, HIGH); 
  digitalWrite(YouToo, HIGH);

  servoself();

}

void loop() {

  int value = analogRead(A0);
  float voltage = value *(5.0/1024.0);

  if(voltage <1.0)
  {
    Serial.println("begin");
    digitalWrite(IBelieve, LOW);
    Serial.println("Iblieve");
    delay(5700);
    for(int i = 0; i<3; i++)
    {
      servoself2();
    }
    digitalWrite(En,HIGH);
    digitalWrite(Thrones, HIGH);
    digitalWrite(Hands, HIGH); 
    digitalWrite(Erdtree, HIGH);
    digitalWrite(Leash, HIGH); 
    digitalWrite(YouToo, HIGH);
    Serial.println("done");
  }

  digitalWrite(IBelieve, HIGH); 

  if(Serial.available()>0)
  {  
    Serial.println("voice");
    voice = Serial.read();
    switch(voice)
    {
      case 49:
        servoself();
        digitalWrite(En,LOW);
        digitalWrite(Thrones, HIGH);
        digitalWrite(Hands, HIGH); 
        digitalWrite(Erdtree, HIGH);
        digitalWrite(Leash, HIGH); 
        digitalWrite(YouToo, HIGH);
        break;

      case 51:
        servoself();
        digitalWrite(En, HIGH);
        digitalWrite(Thrones,LOW);
        break;

      case 52:
        servoself();
        digitalWrite(En, HIGH);
        digitalWrite(Hands,LOW);
        break;

      case 53:
        servoself();
        digitalWrite(En, HIGH);
        digitalWrite(Erdtree,LOW);
        break;

      case 54:
        servoself();
        digitalWrite(En, HIGH);
        digitalWrite(Leash,LOW);
        break;

      case 55:
        servoself();
        digitalWrite(En, HIGH);
        digitalWrite(YouToo,LOW);
        break;
    }
    Serial.println(voice);
  }

}

void servoself()
{
  for(pos = 30; pos < 150; pos += 2){
    myservo.write(pos);
    delay(15);
  }
  for(pos = 150; pos >=30; pos -= 2){
    myservo.write(pos);
    delay(15);
  }
}


void servoself2()
{
  for(pos = 30; pos < 150; pos += 3){
    myservo.write(pos);
    delay(15);
  }
  for(pos = 150; pos >=30; pos -= 3){
    myservo.write(pos);
    delay(15);
  }
}