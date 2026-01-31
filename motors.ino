//motor 1
int buttonPin = 4; 

int num = 5; 
int num2 = 6; 
int num3 = 7; 


//motor 2
int num4 = 8; 
int num5 = 9; 
int num6 = 10; 

int value = 1; 

void setup() {
  pinMode(4, INPUT); 

  int buttonState = digitalRead(4); 
  int check = 0; 

  pinMode(num, OUTPUT); 
  pinMode(num2, OUTPUT); 
  pinMode(num3, OUTPUT); 

  pinMode(num4, OUTPUT); 
  pinMode(num5, OUTPUT); 
  pinMode(num6, OUTPUT); 


if (buttonState == 1) {
      digitalWrite(num2, HIGH); 
  digitalWrite(num5, LOW); 

  digitalWrite(num3, LOW); 
    digitalWrite(num6, HIGH); 

  analogWrite(num, 240); 
    analogWrite(num4, 255); 
  delay(2000);
} 




  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
