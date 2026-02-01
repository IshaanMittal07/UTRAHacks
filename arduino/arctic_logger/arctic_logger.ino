/*
 * Arctic Analytics - Robot Data Logger
 * UTRA Hacks 2026
 *
 * Logs: Color sensor, IR line sensor, Ultrasonic, Motors
 * Dumps JSON over Serial for MongoDB upload
 */

// ============== PIN DEFINITIONS ==============
// Motors (L298N)
#define ENA 5
#define ENB 10
#define IN1 6
#define IN2 7
#define IN3 9
#define IN4 8

// Color Sensor (TCS3200)
#define COLOR_S0 A0
#define COLOR_S1 A1
#define COLOR_S2 A2
#define COLOR_S3 A3
#define COLOR_OUT A4

// IR Line Sensor
#define IR_PIN 2

// Ultrasonic Sensor
#define TRIG_PIN 3
#define ECHO_PIN 4

// ============== DATA STORAGE ==============
#define MAX_READINGS 100
#define LOG_INTERVAL 1000  // Log every 1 second

struct SensorReading {
  unsigned long timestamp;
  int redRaw;
  int greenRaw;
  int blueRaw;
  char colorDetected;  // R, G, B, U (unknown)
  bool lineIsWhite;
  int distanceCm;
  int motorLeftSpeed;
  int motorRightSpeed;
};

SensorReading readings[MAX_READINGS];
int readingCount = 0;
unsigned long sessionStart = 0;
unsigned long lastLogTime = 0;
bool sessionActive = false;

// Color summary counters
int colorCounts[4] = {0, 0, 0, 0};  // R, G, B, Unknown
int whiteCount = 0;
int blackCount = 0;

// Current motor speeds (track these when you set motors)
int currentLeftSpeed = 0;
int currentRightSpeed = 0;

// ============== SENSOR FUNCTIONS ==============

void readColorSensor(int &r, int &g, int &b) {
  // Read Red
  digitalWrite(COLOR_S2, LOW);
  digitalWrite(COLOR_S3, LOW);
  r = pulseIn(COLOR_OUT, LOW, 10000);

  // Read Green
  digitalWrite(COLOR_S2, HIGH);
  digitalWrite(COLOR_S3, HIGH);
  g = pulseIn(COLOR_OUT, LOW, 10000);

  // Read Blue
  digitalWrite(COLOR_S2, LOW);
  digitalWrite(COLOR_S3, HIGH);
  b = pulseIn(COLOR_OUT, LOW, 10000);
}

char detectColor(int r, int g, int b) {
  // Lower value = more of that color
  if (r < (g - 15) && r < b) return 'R';
  if (g < r && g < b) return 'G';
  if (b < r && b < g) return 'B';
  return 'U';  // Unknown
}

bool readLineSensor() {
  return digitalRead(IR_PIN) == HIGH;  // HIGH = white, LOW = black
}

int readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  int distance = duration * 0.034 / 2;
  return min(distance, 400);
}

// ============== MOTOR CONTROL ==============

void setMotors(int leftSpeed, int rightSpeed) {
  currentLeftSpeed = abs(leftSpeed);
  currentRightSpeed = abs(rightSpeed);

  // Left motor
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(ENA, currentLeftSpeed);

  // Right motor
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENB, currentRightSpeed);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  currentLeftSpeed = 0;
  currentRightSpeed = 0;
}

// ============== LOGGING ==============

void logReading() {
  if (readingCount >= MAX_READINGS) return;

  SensorReading &r = readings[readingCount];

  r.timestamp = millis() - sessionStart;
  readColorSensor(r.redRaw, r.greenRaw, r.blueRaw);
  r.colorDetected = detectColor(r.redRaw, r.greenRaw, r.blueRaw);
  r.lineIsWhite = readLineSensor();
  r.distanceCm = readUltrasonic();
  r.motorLeftSpeed = currentLeftSpeed;
  r.motorRightSpeed = currentRightSpeed;

  // Update counters
  switch (r.colorDetected) {
    case 'R': colorCounts[0]++; break;
    case 'G': colorCounts[1]++; break;
    case 'B': colorCounts[2]++; break;
    default: colorCounts[3]++; break;
  }

  if (r.lineIsWhite) whiteCount++;
  else blackCount++;

  readingCount++;
}

void startSession() {
  readingCount = 0;
  sessionStart = millis();
  lastLogTime = millis();
  sessionActive = true;
  colorCounts[0] = colorCounts[1] = colorCounts[2] = colorCounts[3] = 0;
  whiteCount = blackCount = 0;
  Serial.println("SESSION_STARTED");
}

void endSession() {
  sessionActive = false;
  Serial.println("SESSION_ENDED");
}

void dumpJSON() {
  unsigned long duration = 0;
  if (readingCount > 0) {
    duration = readings[readingCount - 1].timestamp;
  }

  Serial.println("{");
  Serial.print("  \"duration_ms\": "); Serial.print(duration); Serial.println(",");
  Serial.print("  \"reading_count\": "); Serial.print(readingCount); Serial.println(",");

  // Color summary
  Serial.println("  \"color_summary\": {");
  Serial.print("    \"red\": "); Serial.print(colorCounts[0]); Serial.println(",");
  Serial.print("    \"green\": "); Serial.print(colorCounts[1]); Serial.println(",");
  Serial.print("    \"blue\": "); Serial.print(colorCounts[2]); Serial.println(",");
  Serial.print("    \"unknown\": "); Serial.print(colorCounts[3]);
  Serial.println("\n  },");

  // Line follow summary
  Serial.println("  \"line_summary\": {");
  Serial.print("    \"white\": "); Serial.print(whiteCount); Serial.println(",");
  Serial.print("    \"black\": "); Serial.print(blackCount);
  Serial.println("\n  },");

  // Readings array
  Serial.println("  \"readings\": [");
  for (int i = 0; i < readingCount; i++) {
    SensorReading &r = readings[i];
    Serial.print("    {\"t\":"); Serial.print(r.timestamp);
    Serial.print(",\"r\":"); Serial.print(r.redRaw);
    Serial.print(",\"g\":"); Serial.print(r.greenRaw);
    Serial.print(",\"b\":"); Serial.print(r.blueRaw);
    Serial.print(",\"color\":\""); Serial.print(r.colorDetected); Serial.print("\"");
    Serial.print(",\"line\":\""); Serial.print(r.lineIsWhite ? "W" : "B"); Serial.print("\"");
    Serial.print(",\"dist\":"); Serial.print(r.distanceCm);
    Serial.print(",\"ml\":"); Serial.print(r.motorLeftSpeed);
    Serial.print(",\"mr\":"); Serial.print(r.motorRightSpeed);
    Serial.print("}");
    if (i < readingCount - 1) Serial.print(",");
    Serial.println();
  }
  Serial.println("  ]");
  Serial.println("}");
  Serial.println("END_JSON");
}

// ============== SETUP ==============

void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Color sensor pins
  pinMode(COLOR_S0, OUTPUT);
  pinMode(COLOR_S1, OUTPUT);
  pinMode(COLOR_S2, OUTPUT);
  pinMode(COLOR_S3, OUTPUT);
  pinMode(COLOR_OUT, INPUT);
  digitalWrite(COLOR_S0, HIGH);
  digitalWrite(COLOR_S1, LOW);

  // IR sensor
  pinMode(IR_PIN, INPUT);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  stopMotors();

  Serial.println("ARCTIC_READY");
}

// ============== LOOP ==============

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    char cmd = Serial.read();
    switch (cmd) {
      case 'S': startSession(); break;
      case 'E': endSession(); break;
      case 'D': dumpJSON(); break;
      case 'C': readingCount = 0; Serial.println("CLEARED"); break;
    }
  }

  // Auto-log during session
  if (sessionActive && (millis() - lastLogTime >= LOG_INTERVAL)) {
    lastLogTime = millis();
    logReading();
  }
}
