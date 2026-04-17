#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>

// --- SENSOR & PINS ---
MPU6050 mpu(Wire);

const int LED_PIN = 13;

//left  
const int IN1 = 6;  
const int IN2 = 5;  
//right
const int IN3 = 10;  
const int IN4 = 9; 

// --- PID PARAMETERS ---
float Kp = 2.0; 
float Kd = 0;  
float Ki = 0;  

float targetAngle = 0.0; 
float error = 0, previousError = 0;
float integral = 0, derivative = 0;
float pidOutput = 0;

float throttle = 0;
float steering = 0;

// --- TIMING ---
unsigned long lastTime = 0; 
unsigned long lastCommandTime = 0;
const int loopTime = 10;    

// --- FUNCTION PROTOTYPES ---
void driveMotors(float leftSide, float rightSide);

void setup() {
  Serial.begin(9600);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Wire.begin();
  
  mpu.begin();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  Serial.println("Calibrating... DO NOT MOVE ROBOT!");
  delay(1000);
  mpu.calcOffsets(); 
  Serial.println("Calibration Complete.");

  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastTime >= loopTime) {
    lastTime = currentTime;

    // 1. Position Sensing
    mpu.update();
    float currentAngle = mpu.getAngleY(); 

    // 2. Safety Kill Switch
    if (abs(currentAngle) > 45) {
      driveMotors(0, 0);  
      integral = 0; 
      digitalWrite(LED_PIN, (millis() / 100) % 2);   
      return;          
    }

    if (Serial.available() > 0) {
      char command = Serial.read();
      lastCommandTime = millis();

      if (command == 'w') throttle = 2.0;  // Forward
      else if (command == 's') throttle = -2.0; // Backward
      else if (command == 'a') steering = 15.0; // Left
      else if (command == 'd') steering = -15.0; // Right
    }
    if (millis() - lastCommandTime > 150) {
      throttle = 0;
      steering = 0;
  }

    // 3. PID Math
    error = targetAngle + throttle - currentAngle;
    
    integral = constrain(integral + error, -255, 255);

    derivative = error - previousError;
    
    pidOutput = (Kp * error) + (Ki * integral) + (Kd * derivative);
    previousError = error; 

    // 4. Actuation
    float leftSide = pidOutput + steering;
    float rightSide = pidOutput - steering;

    driveMotors(pidOutput, pidOutput);

    // 5. Telemetry for Serial Plotter
    Serial.print(currentAngle); 
    Serial.print(","); 
    Serial.println(pidOutput / 10); 
  } 
}

void driveMotors(float leftSide, float rightSide) {
  // --- Left Motor ---
  int leftSpeed = abs(leftSide);
  leftSpeed = constrain(leftSpeed, 10, 255);

  if (leftSide > 0) {
    analogWrite(IN1, leftSpeed); // PWM for speed
    analogWrite(IN2, 0);      // Direction
  } else {
    analogWrite(IN1, 0);      // Direction
    analogWrite(IN2, leftSpeed); // PWM for speedwads
  }

  // --- Right Motor ---
  int rightSpeed = abs(rightSide);
  rightSpeed = constrain(rightSpeed, 10, 255);

  if (rightSide > 0) {
    analogWrite(IN3, 0);
    analogWrite(IN4, rightSpeed);
  } else {
    analogWrite(IN3, rightSpeed);
    analogWrite(IN4, 0);
  }
}