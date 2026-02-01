const int trigPin = 3; 
const int echoPin = 4; 
double time = 0; 
double distance = 0; 


void forward (bool check) { //forward method 

}

void backward (bool check) { //backwards method 


}

void right (bool check) { //right turn method 


}

void left (bool check) { //left turn method 


}

void sensor () { // Ultrasonic sensor 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(5); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  time = pulseIn(echoPin, HIGH); 
  distance = time * 0.034 / 2 



}


void setup() {
  pinMode()

}

void loop() {
  digitalWrite (8, HIGH); 
  delay (1000); 
  digitalWrite(8 LOW); 
  delay(500);   
}
