#include "Energia.h"

#line 1 "C:/Users/mahon/workspace_v8/MORSE/Button.ino"






























void setup();
void loop();
void dash();
void dot();

#line 31
const int buttonPin1 = 33;     
const int buttonPin2 = 32;
const int BUZZER =  40;      


int buttonState1 = 0;         

int buttonState2 = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(BUZZER, OUTPUT);
  
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
}

void loop(){
  
  buttonState1 = digitalRead(buttonPin1);

  buttonState2 = digitalRead(buttonPin2);

  
  
  if (buttonState1 != HIGH) {
    
      dot();
  }

  if (buttonState2 != HIGH){
      dash();
  }

}


void dash() {
  digitalWrite(BUZZER, HIGH);
  delay(300);
  digitalWrite(BUZZER, LOW);
  delay(50);
}



void dot() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
  delay(50);
}



