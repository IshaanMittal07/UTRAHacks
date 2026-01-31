   //////////////////////////////////////////////
  //       Arduino Color Sensor Tutorial      //
 //          with ST7735 TFT LCD             //
//           http://www.educ8s.tv           //
/////////////////////////////////////////////


int S0 = 4;  
int S1 = 5;  
int S2 = 6;  
int S3 = 7; 
int sensorOut = 8; 


int red = 0; 
int green = 0; 
int blue = 0; 

void setup() {
  pinMode(S0, OUTPUT); 
  pinMode(S1, OUTPUT); 
  pinMode(S2, OUTPUT); 
  pinMode(S3, OUTPUT); 
  pinMode(sensorOut, INPUT); 


  digitalWrite(S0, HIGH); 
  digitalWrite(S1, LOW); 

  Serial.begin(9600);
}

void loop() {
  digitalWrite(S2, LOW); 
  digitalWrite(S3, LOW); 
  red = pulseIn(sensorOut, LOW); 

  digitalWrite(S2, HIGH); 
  digitalWrite(S3, HIGH); 
  green = pulseIn(sensorOut, LOW); 

  digitalWrite(S2, LOW); 
  digitalWrite(S3, HIGH); 
  blue = pulseIn(sensorOut, LOW); 

  Serial.print("R = "); 
  Serial.print(red); 
  Serial.print("G = "); 
  Serial.print(green); 
  Serial.print("B = "); 
  Serial.print(blue); 

  if (red < green && red < blue) {
    Serial.println("REEEEEEEEEEED"); 
  }
  else if (green < red && green < blue) {
    Serial.println("GREEN"); 
  }else if (blue < green && blue < red) {
    Serial.println("BLUE"); 
  }

  else {
    Serial.println("ERM");
  }

  delay(500); 

}








  