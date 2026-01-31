// int irSensorPin = 2; // Connected to the "Out" pin of the sensor

// void setup() {
//   pinMode(irSensorPin, INPUT);
//   Serial.begin(9600);
// }

// void loop() {
//   int val = digitalRead(irSensorPin);
//   if (val == HIGH) {
//     Serial.println("White detected!");
//   } else {
//     Serial.println("Black detected!");
//   }
// }




//MOTOR1 PINS
int ena = 5;
int enb = 8;
int in1 = 6;
int in2 = 7;
int in3 = 3;
int in4 = 4;

void move(char direction, double dist){
  if (distance == "f"){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
    analogWrite(ena, 255);
    analogWrite(enb, 255);
    delay(dist);
  }
  if (distance == "r"){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    analogWrite(ena, 255);
    analogWrite(enb, 255);
    delay(dist);
  }
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(10);
}



void setup() {

  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //CLOCKWISE MAX SPEED
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(ena, 255);
  analogWrite(enb, 255);
  delay(2000);
  
  //STOP
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(2000);

  //COUNTERCLOCKWISE MAX SPEED
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(ena, 255);
  analogWrite(enb, 255);
  delay(2000);

  //STOP
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(2000);

  //CLOCKWISE HALF SPEED
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  analogWrite(ena, 127);
  analogWrite(enb, 255);
  delay(2000);

  //STOP
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
  delay(2000);

  //COUNTERCLOCKWISE MAX SPEED
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(ena, 127);
  analogWrite(enb, 127);
  delay(2000);

  //STOP
  
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
}

void loop() { 
}



// // Pin Definitions
// int S0 = 4;  
// int S1 = 5;  
// int S2 = 6;  
// int S3 = 7; 
// int sensorOut = 8; 

// // Variables for RGB values
// int redRaw = 0; 
// int greenRaw = 0; 
// int blueRaw = 0; 

// void setup() {
//   // Setting the outputs
//   pinMode(S0, OUTPUT);
//   pinMode(S1, OUTPUT);
//   pinMode(S2, OUTPUT);
//   pinMode(S3, OUTPUT);
  
//   // Setting sensorOut as input
//   pinMode(sensorOut, INPUT);
  
//   // Setting Frequency scaling to 20%
//   // This is the most accurate for Arduino
//   digitalWrite(S0, HIGH);
//   digitalWrite(S1, LOW); 
  
//   Serial.begin(9600);
//   Serial.println("Color Sensor Initialized...");
// }

// void loop() {
//   // 1. Read RED frequency
//   digitalWrite(S2, LOW);
//   digitalWrite(S3, LOW);
//   redRaw = pulseIn(sensorOut, LOW);
//   delay(10); // Short pause for stability

//   // 2. Read GREEN frequency
//   digitalWrite(S2, HIGH);
//   digitalWrite(S3, HIGH);
//   greenRaw = pulseIn(sensorOut, LOW);
//   delay(10);

//   // 3. Read BLUE frequency
//   digitalWrite(S2, LOW);
//   digitalWrite(S3, HIGH);
//   blueRaw = pulseIn(sensorOut, LOW);
//   delay(10);

//   // --- Print Raw Data for Debugging ---
//   Serial.print("R: "); Serial.print(redRaw);
//   Serial.print(" | G: "); Serial.print(greenRaw);
//   Serial.print(" | B: "); Serial.print(blueRaw);

//   // --- Detection Logic ---
//   // Remember: A SMALLER raw number means MORE of that color.
  
//   // Checking for Red
//   // (We add a small 'buffer' of 10-20 to prevent green-bias)
//   if (redRaw < (greenRaw - 15) && redRaw < blueRaw) {
//     Serial.println(" -> DETECTED: RED");
//   } 
//   // Checking for Green
//   else if (greenRaw < redRaw && greenRaw < blueRaw) {
//     Serial.println(" -> DETECTED: GREEN");
//   } 
//   // Checking for Blue
//   else if (blueRaw < redRaw && blueRaw < greenRaw) {
//     Serial.println(" -> DETECTED: BLUE");
//   } 
//   else {
//     Serial.println(" -> DETECTED: UNKNOWN");
//   }

//   delay(500); // Wait half a second before next reading
// }



// const int trigPin = 3; 
// const int echoPin = 2; 
// double time = 0; 
// double distance = 0;

// void sensor () { // Ultrasonic sensor 
//   digitalWrite(trigPin, LOW); 
//   delayMicroseconds(5); 
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);
//   time = pulseIn(echoPin, HIGH); 
//   distance = time * 0.034 / 2;
// }

// void setup() {
//   pinMode(trigPin, OUTPUT);
//   pinMode(echoPin, INPUT);
//   Serial.begin(9600);
// }

// void loop() {
//   distance = 0;
//   sensor();
//   Serial.print("Dist: ");
//   Serial.println(distance);
//   delay(100);   
// }