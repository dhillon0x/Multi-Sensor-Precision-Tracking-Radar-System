#include <Servo.h>
#include <Wire.h>


#define SERVO_PIN 9
#define SWEEP_TRIG 7
#define SWEEP_ECHO 8
#define LONG_RANGE_PIN A0   // Secondary precision sensor interface (LiDAR / ToF)


const int greenLED = 2;  
const int redLED = 3;    
const int orangeLED = 4;  
const int buzzerPin = 5;  
const int blueLED = 13;   
const int whiteLED = 12;  


Servo radar;
int angle = 90;
int targetAngle = 90;
bool targetLocked = false;
int sweepDir = 1;


unsigned long lastBeep = 0;
unsigned long lastBlink = 0;
unsigned long lastBlueBlink = 0;
unsigned long lastWhiteBlink = 0;


bool redState = false;
bool orangeState = false;
bool blueState = false;
bool whiteState = false;
int stableCount = 0;

void setup() {
  Serial.begin(9600);
  
  radar.attach(SERVO_PIN);
  
  pinMode(SWEEP_TRIG, OUTPUT);
  pinMode(SWEEP_ECHO, INPUT);
  pinMode(LONG_RANGE_PIN, INPUT);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(orangeLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  radar.write(90);
  delay(1000);
  Serial.println("System Initialized: Angle,SweepDistance,LongRangeVal,Status");
}

void loop() {
  int sweepDist = getUltrasonicDistance();
  int longRangeValue = analogRead(LONG_RANGE_PIN);

  if (!targetLocked) {
    angle += 4 * sweepDir; 
    if (angle >= 180) { angle = 180; sweepDir = -1; }
    if (angle <= 0)   { angle = 0;   sweepDir = 1;  }
    radar.write(angle);

    if (sweepDist < 30 && sweepDist > 2) { 
      stableCount++;
      if (stableCount >= 3) { 
        targetAngle = angle;
        targetLocked = true;
        stableCount = 0;
      }
    } else {
      stableCount = 0;
    }

    digitalWrite(greenLED, HIGH);
    digitalWrite(blueLED, LOW);
    digitalWrite(whiteLED, LOW);

    if (millis() - lastBeep >= 400) {
      tone(buzzerPin, 800, 100);
      redState = !redState;
      digitalWrite(redLED, redState);
      lastBeep = millis();
    }

    if (millis() - lastBlink >= 800) {
      orangeState = !orangeState;
      digitalWrite(orangeLED, orangeState);
      lastBlink = millis();
    }

  } else {
    int error = targetAngle - angle;
    if (sweepDist < 50) targetAngle = angle;
    
    if (abs(error) > 8) {
      angle += (error > 0) ? 6 : -6; 
      angle = constrain(angle, 0, 180);
    }
    radar.write(angle);

    if (sweepDist > 55 && longRangeValue < 200) { 
      targetLocked = false;
      sweepDir = 1;
    }

    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    tone(buzzerPin, 1000);

    if (millis() - lastBlink >= 250) {
      orangeState = !orangeState;
      digitalWrite(orangeLED, orangeState);
      lastBlink = millis();
    }

    int safeDist = constrain(sweepDist, 5, 50);
    int blueInterval = map(safeDist, 5, 50, 100, 600);
    if (millis() - lastBlueBlink >= blueInterval) {
      blueState = !blueState;
      digitalWrite(blueLED, blueState);
      lastBlueBlink = millis();
    }

    if (sweepDist < 5) {
      if (millis() - lastWhiteBlink >= 100) {
        whiteState = !whiteState;
        digitalWrite(whiteLED, whiteState);
        lastWhiteBlink = millis();
      }
      for (int i = 0; i < 3; i++) {
        tone(buzzerPin, 1500, 80);
        delay(60);
        tone(buzzerPin, 2000, 80);
        delay(60);
      }
    } else {
      if (whiteState) {
        whiteState = false;
        digitalWrite(whiteLED, LOW);
      }
    }
  }

  Serial.print(angle);
  Serial.print(",");
  Serial.print(sweepDist);
  Serial.print(",");
  Serial.print(longRangeValue);
  Serial.print(",");
  Serial.println(targetLocked ? "LOCKED" : "SWEEP");

  delay(70);
}

int getUltrasonicDistance() {
  digitalWrite(SWEEP_TRIG, LOW);   
  delayMicroseconds(2);
  digitalWrite(SWEEP_TRIG, HIGH);  
  delayMicroseconds(10);
  digitalWrite(SWEEP_TRIG, LOW);
  long duration = pulseIn(SWEEP_ECHO, HIGH, 30000);
  return (duration == 0) ? 999 : duration * 0.034 / 2;
}
