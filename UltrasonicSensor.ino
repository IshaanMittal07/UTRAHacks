//Source: https://www.youtube.com/watch?v=KGwtit2bFyo

const int trigPin = 10; 
const int echoPin = 9; 
const int ledPin = 3; 

float duration, distance; 

void setup()
{
    Serial.begin(9600); 

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT); 

}

void loop()
{
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH); 
  
  distance = (duration*0.034) / 2; 
  
  Serial.println(distance); 

  if (distance < 10) {
    analogWrite(ledPin, 255 - distance); 

    delay(100); 
  }

  else if (distance > 10 && distance < 15) {
          analogWrite(ledPin, 255 - (distance + 100)); 

              delay(100); 

  }

  else if (distance > 15 && distance < 20) {
              analogWrite(ledPin, 255 - (distance + 150)); 

                  delay(100); 

  }

  else if (distance > 20 && distance < 30) {
    analogWrite(ledPin, 255 - (distance + 200)); 

        delay(100); 


  }

  else if (distance >= 255) {
    analogWrite(ledPin, 0); 

        delay(100); 

  }
}