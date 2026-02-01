int S0 = 2; // green
int S1 = 3; // oranage
int S2 = 4; // red
int S3 = 5; // yellow
int sensorOut = 6; // blue
int redRaw = 0;
int greenRaw = 0;
int blueRaw = 0;


char readColour();
void LineFollow();

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  Serial.begin(9600);
  Serial.println("Color Sensor Initialized...");
}

char readColour()
{
  // 1. Read RED frequency
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redRaw = pulseIn(sensorOut, LOW);
  delay(10); // Short pause for stability

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

  // --- Print Raw Data for Debugging ---
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
  else if (blueRaw < redRaw && blueRaw < greenRaw) {
    Serial.println(" -> DETECTED: BLUE");
    return 'b';
  }
  else {
    Serial.println(" -> DETECTED: UNKNOWN");
    return 'w';
  }
  delay(100); // Wait half a second before next reading
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
