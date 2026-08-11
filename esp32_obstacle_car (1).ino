/*
  ESP32 Obstacle-Avoiding Car
  ---------------------------
  Components:
    - ESP32 Dev Board
    - L298N Motor Driver
    - 2x/4x DC Motors
    - HC-SR04 Ultrasonic Sensor
    - SG90 Servo (sensor sweep)

  Behavior:
    Car drives forward. When an obstacle is detected within
    STOP_DISTANCE, it stops, sweeps the ultrasonic sensor left/right
    to find the clearest path, then turns and continues.
*/

#include <ESP32Servo.h>

// ---------- Motor Driver Pins ----------
#define ENA 14   // Left motor speed (PWM)
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENB 32   // Right motor speed (PWM)

// ---------- Ultrasonic Sensor Pins ----------
#define TRIG_PIN 5
#define ECHO_PIN 18

// ---------- Servo Pin ----------
#define SERVO_PIN 13

// ---------- PWM (LEDC) Config ----------
#define PWM_FREQ 1000
#define PWM_RES  8       // 8-bit -> 0-255
#define PWM_CH_A 0
#define PWM_CH_B 1

// ---------- Tunables ----------
const int STOP_DISTANCE = 20;   // cm - stop and decide when closer than this
const int SPEED = 200;          // 0-255
const int TURN_TIME = 500;      // ms to turn ~90 degrees, tune for your car

Servo sensorServo;

// ---------- Motor Control Functions ----------
void motorsStop() {
  ledcWrite(PWM_CH_A, 0);
  ledcWrite(PWM_CH_B, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void motorsForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(PWM_CH_A, speed);
  ledcWrite(PWM_CH_B, speed);
}

void motorsBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CH_A, speed);
  ledcWrite(PWM_CH_B, speed);
}

void turnLeft(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(PWM_CH_A, speed);
  ledcWrite(PWM_CH_B, speed);
}

void turnRight(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(PWM_CH_A, speed);
  ledcWrite(PWM_CH_B, speed);
}

// ---------- Ultrasonic Distance ----------
long getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout (~5m range)
  if (duration == 0) return 999; // no echo = treat as clear

  long distance = duration * 0.0343 / 2; // speed of sound = 343 m/s
  return distance;
}

// Average a few readings for stability
long getStableDistance() {
  long sum = 0;
  int samples = 3;
  for (int i = 0; i < samples; i++) {
    sum += getDistanceCM();
    delay(20);
  }
  return sum / samples;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  ledcSetup(PWM_CH_A, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, PWM_CH_A);
  ledcSetup(PWM_CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENB, PWM_CH_B);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  sensorServo.setPeriodHertz(50);
  sensorServo.attach(SERVO_PIN, 500, 2400);
  sensorServo.write(90); // center

  motorsStop();
  delay(1000);
}

// ---------- Main Loop ----------
void loop() {
  sensorServo.write(90); // look forward
  delay(200);
  long distance = getStableDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > STOP_DISTANCE) {
    motorsForward(SPEED);
  } else {
    motorsStop();
    delay(200);

    // Look right
    sensorServo.write(20);
    delay(400);
    long distRight = getStableDistance();

    // Look left
    sensorServo.write(160);
    delay(400);
    long distLeft = getStableDistance();

    // Recenter
    sensorServo.write(90);
    delay(300);

    if (distLeft > distRight && distLeft > STOP_DISTANCE) {
      turnLeft(SPEED);
      delay(TURN_TIME);
    } else if (distRight > STOP_DISTANCE) {
      turnRight(SPEED);
      delay(TURN_TIME);
    } else {
      // Both blocked, back up then turn
      motorsBackward(SPEED);
      delay(400);
      turnRight(SPEED);
      delay(TURN_TIME * 2);
    }
    motorsStop();
  }
}
