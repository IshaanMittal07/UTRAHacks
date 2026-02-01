// // Motor A (Left)
// int ena = 8;
// int in1 = 9;
// int in2 = 10;

// // Motor B (Right)
// int enb = 13;
// int in3 = 11;
// int in4 = 12;

// void setup() {
//   pinMode(ena, OUTPUT);
//   pinMode(enb, OUTPUT);
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);
//   pinMode(in3, OUTPUT);
//   pinMode(in4, OUTPUT);

//   // --- MOVE FORWARD ---
//   // Left Motor
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, HIGH);
//   analogWrite(ena, 255); // Full Speed

//   // Right Motor
//   digitalWrite(in3, HIGH);
//   digitalWrite(in4, LOW);
//   analogWrite(enb, 255); // Full Speed
// }

// void loop() {
//   // Motors will keep running forward because they were turned on in setup()
// }
#include <Servo.h>
Servo myServo;  // Create a servo object
int ena = 8;
int enb = 13;
int in1 = 9;
int in2 = 10;
int in3 = 11;
int in4 = 12;
int S0 = 2; // green
int S1 = 3; // oranage
int S2 = 4; // red
int S3 = 5; // yellow
int sensorOut = 6; // blue
int redRaw = 0;
int greenRaw = 0;
int blueRaw = 0;

char readColour();
void LineFollow(double total_dist);
void tankMove(char direction, double dist);

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  Serial.println("Color Sensor Initialized...");
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  myServo.attach(7);

  myServo.write(140);
  Serial.begin(9600);
  delay(7000);
  tankMove('f',6000);
  tankMove('L',1800);
  tankMove('f',2000);
  delay(2000);
  LineFollow(40);
  delay(1000);
  tankMove('r',1000);
  delay(1000);
  tankMove('L',1000);
  tankMove('f',2500);
  tankMove('R',1000);
  delay(1000);
  tankMove('L',2500);
  myServo.write(0);
  delay(1000);
  tankMove('r',3000);
  delay(1000);
  myServo.write(120);
  delay(1000);
  tankMove('f',3000);
  delay(1000);
  tankMove('R',2500);
  tankMove('f',2000);
  tankMove('R',200);
  delay(1000);
  LineFollow(4);
  delay(1000);
  // tankMove('R',2000);
  // LineFollow(7);
  // delay(1000);
  // tankMove('R',2000);
  // tankMove('f',3000);
  // tankMove('L',2000);
  // tankMove('f',6000);
  // LineFollow(30);
  // delay(1000);

}


void tankMove(char direction, double dist) {
  if (direction == 'f') { // Forward
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(ena, 255);
    analogWrite(enb, 60);
    delay(dist);
  }
  else if (direction == 'b') { // Backward
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(ena, 255);
    analogWrite(enb, 60);
    delay(dist);
  }
  else if (direction == 'R') { // Right
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(ena, 255);
    analogWrite(enb, 60);
    delay(dist);
  }
  else { // Left
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(ena, 255);
    analogWrite(enb, 50);
    delay(dist);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(10);
}


char readColour()
{
  // 1. Read RED frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redRaw = pulseIn(sensorOut, LOW);
  delay(10);

  // 2. Read GREEN frequency
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenRaw = pulseIn(sensorOut, LOW);
  delay(10);

  // 3. Read BLUE frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueRaw = pulseIn(sensorOut, LOW);
  delay(10);

  Serial.print("R: "); Serial.print(redRaw);
  Serial.print(" | G: "); Serial.print(greenRaw);
  Serial.print(" | B: "); Serial.print(blueRaw);

  if (redRaw < (greenRaw - 15) && redRaw < blueRaw) {
    Serial.println(" -> DETECTED: RED");
    return 'r';
  }
  else if (greenRaw < redRaw && greenRaw < blueRaw) {
    Serial.println(" -> DETECTED: GREEN");
    return 'g';
  }
  else if ((blueRaw+20) < redRaw && (blueRaw+20) < greenRaw) {
    Serial.println(" -> DETECTED: BLUE");
    return 'b';
  }
  else {
    Serial.println(" -> DETECTED: UNKNOWN");
    return 'w';
  }
  delay(100);
}

void LineFollow(double total_dist) {
  int leftMax = 255; 
  int leftMin = 0;

  int rightMax = 75;
  int rightMin = 45;

  long endTime = millis() + (total_dist * 1000);
  
  while (millis() < endTime) {
    char val = readColour();

    if (val == 'b') break; 

    // Directions (Forward)
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    if (val == 'r' || val == 'g') {
      analogWrite(ena, leftMax); 
      analogWrite(enb, rightMin); 
    } 
    else {
      analogWrite(ena, leftMin); 
      analogWrite(enb, rightMax);
    }
    delay(30); 
  }

  analogWrite(ena, 0);
  analogWrite(enb, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void loop(){

}