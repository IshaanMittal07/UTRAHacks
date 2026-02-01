#include <Servo.h>
Servo myServo;  // Create a servo object
void setup() {
  myServo.attach(11);  // Connect the signal wire to Digital Pin 9
}

void loop() {
  myServo.write(0);   // Move to 0 degrees
  delay(1000);        // Wait 1 second

  myServo.write(90);  // Move to 90 degrees
  delay(1000);

  myServo.write(180); // Move to 180 degrees
  delay(1000);
}
