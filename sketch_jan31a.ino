// // IR SENSOR CODE
// int lineFollow = A0;  // Connected to the "Out" pin of the sensor
// void setup() {
//   pinMode(lineFollow, INPUT);
//   Serial.begin(9600);
//   // oneMove(20);
// }

// void loop() {
//   int val = digitalRead(lineFollow);
//   if (val == HIGH){
//     Serial.println("White");
//   }
//   else{
//     Serial.println("Black");
//   }
//   delay(10);
// }





// // ARM
// #include <Servo.h>
// Servo myServo;  // Create a servo object
// void setup() {
//   myServo.attach(11);  // Connect the signal wire to Digital Pin 9
// }
// void loop() {
//   myServo.write(0);   // Move to 0 degrees
//   delay(1000);        // Wait 1 second

//   myServo.write(90);  // Move to 90 degrees
//   delay(1000);

//   myServo.write(180); // Move to 180 degrees
//   delay(1000);
// }








// //MOTOR1 PINS
// int ena = 5;
// int enb = 8;
// int in1 = 6;
// int in2 = 7;
// int in3 = 3;
// int in4 = 4;

// void move(char direction, double dist) {
//   if (distance == 'f') {
//     digitalWrite(in1, HIGH);
//     digitalWrite(in2, LOW);
//     digitalWrite(in3, HIGH);
//     digitalWrite(in4, LOW);
//     analogWrite(ena, 255);
//     analogWrite(enb, 255);
//     delay(dist);
//   }
//   if (distance == 'r') {
//     digitalWrite(in1, LOW);
//     digitalWrite(in2, HIGH);
//     digitalWrite(in3, LOW);
//     digitalWrite(in4, HIGH);
//     analogWrite(ena, 255);
//     analogWrite(enb, 255);
//     delay(dist);
//   }
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, LOW);
//   delay(10);
// }




// LINE FOLLOW (RED)
char readcolourRed() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redRaw = pulseIn(sensorOut, LOW);
  delay(10);  // Short pause for stability
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
  if (redRaw < (greenRaw - 15) && redRaw < blueRaw) {
    Serial.println(" -> DETECTED: RED");
    return 'r';
  } 
  else {
    Serial.println(" -> DETECTED: UNKNOWN");
    return 'w';
  }
}


void LineFollow(double total_dist) {
  int mainBase = 100;
  int lowerBase = 80;
  total_dist = total_dist * 1000;
  while (total_dist > 0) {
    char val = readcolour();
    mainBase = 100;
    lowerBase = 80;
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    if (val == 'r' || val == 'g') {
      analogWrite(ena, mainBase);
      analogWrite(enb, lowerBase);
    } 
    else if(val == 'b'){
      break;
    }
    else {
      analogWrite(ena, lowerBase);
      analogWrite(enb, mainBase);
    }
    total_dist = total_dist - 100;
    delay(100);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(100);
  return;
}


// LINE FOLLOW (GREEN)
char readcolour() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redRaw = pulseIn(sensorOut, LOW);
  delay(10);  // Short pause for stability

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

  if (greenRaw < redRaw && greenRaw < blueRaw) {
    Serial.println(" -> DETECTED: GREEN");
    return 'g';
  }
  else if (blueRaw < redRaw && blueRaw < greenRaw) {
    Serial.println(" -> DETECTED: BLUE");
    return 'b';
  } else {
    Serial.println(" -> DETECTED: UNKNOWN");
    return 'w';
  }
}


void LineFollow(double total_dist) {
  int mainBase = 100;
  int lowerBase = 80;
  total_dist = total_dist * 1000;
  while (total_dist > 0) {
    char val = readcolourBlue();
    mainBase = 100;
    lowerBase = 80;
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    if (val == 'g') {
      analogWrite(ena, mainBase);
      analogWrite(enb, lowerBase);
    } 
    else if( val == 'b'){
      break;
    }
    else {
      analogWrite(ena, lowerBase);
      analogWrite(enb, mainBase);
    }
    total_dist = total_dist - 100;
    delay(100);
  }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(100);
  return;
}








// // DRIVE

// void setup() {

//   pinMode(ena, OUTPUT);
//   pinMode(enb, OUTPUT);
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);
//   pinMode(in3, OUTPUT);
//   pinMode(in4, OUTPUT);

//   //CLOCKWISE MAX SPEED
//   digitalWrite(in1, HIGH);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, HIGH);
//   digitalWrite(in4, LOW);
//   analogWrite(ena, 255);
//   analogWrite(enb, 255);
//   delay(2000);

//   //STOP
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, LOW);
//   delay(2000);

//   //COUNTERCLOCKWISE MAX SPEED
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, HIGH);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, HIGH);
//   analogWrite(ena, 255);
//   analogWrite(enb, 255);
//   delay(2000);

//   //STOP
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, LOW);
//   delay(2000);

//   //CLOCKWISE HALF SPEED
//   digitalWrite(in1, HIGH);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, HIGH);
//   digitalWrite(in4, LOW);
//   analogWrite(ena, 127);
//   analogWrite(enb, 255);
//   delay(2000);

//   //STOP
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, LOW);
//   delay(2000);

//   //COUNTERCLOCKWISE MAX SPEED
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, HIGH);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, HIGH);
//   analogWrite(ena, 127);
//   analogWrite(enb, 127);
//   delay(2000);

//   //STOP

//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, LOW);
// }

// void loop() {
// }



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






// // Ultrasonic
// const int trigPin = A0;
// const int echoPin = A1;
// long duration = 0; // Changed from 'time' to 'duration'
// double distance = 0;

// void sensor() {
//   // Ensure the pin is low first
//   digitalWrite(trigPin, LOW);
//   delayMicroseconds(2);

//   // Trigger the 10 microsecond pulse
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);

//   // Read the bounce-back time
//   duration = pulseIn(echoPin, HIGH);

//   // Calculate distance in cm
//   distance = duration * 0.034 / 2;
// }

// void setup() {
//   pinMode(trigPin, OUTPUT);
//   pinMode(echoPin, INPUT);
//   Serial.begin(9600);
// }

// void loop() {
//   sensor(); // Updates the 'distance' variable

//   Serial.print("Disxxt: ");
//   Serial.print(distance);
//   Serial.println(" cm");

//   delay(100);
// }





// // Scracth

// ir Line follow
// void oneMove(double total_dist) {
//   int mainBase = 100;
//   int lowerBase = 80;
//   total_dist = total_dist * 1000;
//   while (total_dist > 0) {
//     int val = digitalRead(lineFollow);
//     mainBase = 100;
//     lowerBase = 80;
//     digitalWrite(in1, HIGH);
//     digitalWrite(in2, LOW);
//     digitalWrite(in3, HIGH);
//     digitalWrite(in4, LOW);
//     if (val == HIGH) {
//       analogWrite(ena, mainBase);
//       analogWrite(enb, lowerBase);
//     } else {
//       analogWrite(ena, lowerBase);
//       analogWrite(enb, mainBase);
//     }
//     total_dist = total_dist - 100;
//     delay(100);
//   }
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   digitalWrite(in3, LOW);
//   digitalWrite(in4, LOW);
//   delay(100);
//   return;
// }