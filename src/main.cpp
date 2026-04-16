#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>

// --- SENSOR & PINS ---
MPU6050 mpu(Wire);

//left
const int ENA = 5;  
const int IN1 = 6;  
const int IN2 = 7;  
//right
const int ENB = 9;  
const int IN3 = 8;  
const int IN4 = 10; 

// --- PID PARAMETERS ---
float Kp = 15.0; 
float Kd = 8.0;  
float Ki = 0.5;  

float targetAngle = 0.0; 
float error = 0, previousError = 0;
float integral = 0, derivative = 0;
float pidOutput = 0;

float throttle = 0;

// --- TIMING ---
unsigned long lastTime = 0; 
const int loopTime = 10;    

// --- FUNCTION PROTOTYPES ---
void driveMotors(float speedOutput);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  mpu.begin();

  Serial.println("Calibrating... DO NOT MOVE ROBOT!");
  delay(1000);
  mpu.calcOffsets(); 
  Serial.println("Calibration Complete.");

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
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
      driveMotors(0);  
      integral = 0;    
      return;          
    }

    // 3. PID Math
    error = targetAngle + throttle - currentAngle;
    
    integral = integral + error;
    if (integral > 255) integral = 255; // Anti-windup
    if (integral < -255) integral = -255;

    derivative = error - previousError;
    
    pidOutput = (Kp * error) + (Ki * integral) + (Kd * derivative);
    previousError = error; 

    // 4. Actuation
    driveMotors(pidOutput);

    // 5. Telemetry for Serial Plotter
    Serial.print(currentAngle); 
    Serial.print(","); 
    Serial.println(pidOutput / 10); 
  } 
}

void driveMotors(float speedOutput) {
  bool forward = (speedOutput > 0);
  int speed = abs(speedOutput);

  // Motor Deadband Compensation
  if (speed > 0 && speed < 40) speed = 40; 
  if (speed > 255) speed = 255;

  if (forward) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}