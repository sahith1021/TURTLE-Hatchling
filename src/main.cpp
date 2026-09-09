#include <Arduino.h>
#include <TurtleReceiver.h>
#include <ESP32Servo.h>

// --- 1. SET UP THE CONTROLLER ---
NetController controller; 

// --- 2. DRIVE PINS (Driver #1) ---
int leftIN1 = 27;
int leftIN2 = 26;
int rightIN3 = 25;
int rightIN4 = 33;

// --- 3. LEVER PINS (Driver #2) ---
int leverIN1 = 16;
int leverIN2 = 17;

// --- 4. SERVO SETUP ---
Servo myServo;
int servoPin = 18;
int servoAngle = 90; 

// --- 5. ULTRASONIC & LED SETUP ---
int trigPin = 5;
int echoPin = 15;
int ledPin = 2;
int triggerDistance = 10; // cm

void setup() {
  Serial.begin(115200);
  
  pinMode(leftIN1, OUTPUT); pinMode(leftIN2, OUTPUT);
  pinMode(rightIN3, OUTPUT); pinMode(rightIN4, OUTPUT);
  pinMode(leverIN1, OUTPUT); pinMode(leverIN2, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(servoAngle);

  controller.controllerSetup();
  printMacAddress(); 
}

void loop() {
  // A: TANK DRIVE
  float leftStickY = controller.getJoy1Y();
  float rightStickY = controller.getJoy2Y();

  if (leftStickY > 0.5) {         
    digitalWrite(leftIN1, HIGH); digitalWrite(leftIN2, LOW);
  } else if (leftStickY < -0.5) { 
    digitalWrite(leftIN1, LOW); digitalWrite(leftIN2, HIGH);
  } else {                        
    digitalWrite(leftIN1, LOW); digitalWrite(leftIN2, LOW);
  }

  if (rightStickY > 0.5) {        
    digitalWrite(rightIN3, HIGH); digitalWrite(rightIN4, LOW);
  } else if (rightStickY < -0.5) { 
    digitalWrite(rightIN3, LOW); digitalWrite(rightIN4, HIGH);
  } else {                        
    digitalWrite(rightIN3, LOW); digitalWrite(rightIN4, LOW);
  }

  // B: LEVER MOTOR
  if (controller.getRt()) {
    digitalWrite(leverIN1, HIGH); digitalWrite(leverIN2, LOW);
  } else if (controller.getLt()) {
    digitalWrite(leverIN1, LOW); digitalWrite(leverIN2, HIGH);
  } else {
    digitalWrite(leverIN1, LOW); digitalWrite(leverIN2, LOW);
  }

  // C: MICRO SERVO
  if (controller.getRb()) { 
    servoAngle += 3; 
    if (servoAngle > 180) servoAngle = 180; 
    myServo.write(servoAngle);
  } else if (controller.getLb()) {
    servoAngle -= 3; 
    if (servoAngle < 0) servoAngle = 0; 
    myServo.write(servoAngle);
  }

  // D: ULTRASONIC SENSOR & LED
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); 
  float distance = (duration * 0.0343) / 2;

  if (distance < triggerDistance && duration > 0) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(15); 
}