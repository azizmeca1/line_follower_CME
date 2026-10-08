#include <Arduino.h>
#include <QTRSensors.h>

QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
const uint8_t qtrPins[SensorCount] = {32, 33, 25, 26, 27, 14, 34, 13};

const int pinPWMA = 18;
const int pinAIN1 = 19;
const int pinAIN2 = 21;

const int pinPWMB = 17;
const int pinBIN1 = 16;
const int pinBIN2 = 4;

const int pinSTBY = 23;

const int baseSpeed = 130;
const int maxSpeed = 200;

float Kp = 0.04;
float Ki = 0.00;
float Kd = 0.25;

int lastError = 0;
int integral = 0;

void setMotorSpeeds(int speedLeft, int speedRight) {
  digitalWrite(pinSTBY, HIGH);

  if (speedLeft >= 0) {
    digitalWrite(pinAIN1, HIGH);
    digitalWrite(pinAIN2, LOW);
  } else {
    digitalWrite(pinAIN1, LOW);
    digitalWrite(pinAIN2, HIGH);
    speedLeft = -speedLeft;
  }
  speedLeft = constrain(speedLeft, 0, maxSpeed);
  analogWrite(pinPWMA, speedLeft);

  if (speedRight >= 0) {
    digitalWrite(pinBIN1, HIGH);
    digitalWrite(pinBIN2, LOW);
  } else {
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, HIGH);
    speedRight = -speedRight;
  }
  speedRight = constrain(speedRight, 0, maxSpeed);
  analogWrite(pinPWMB, speedRight);
}

void setup() {
  Serial.begin(115200);

  pinMode(pinPWMA, OUTPUT);
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinPWMB, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinSTBY, OUTPUT);

  digitalWrite(pinSTBY, LOW);

  qtr.setTypeAnalog();
  qtr.setSensorPins(qtrPins, SensorCount);

  Serial.println("==========================================");
  Serial.println("  CALIBRATION : Balayez la ligne noire !  ");
  Serial.println("==========================================");

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(10);
  }

  Serial.println("Calibration terminée ! Démarrage du suivi...");
  delay(2000);
}

void loop() {
  uint16_t position = qtr.readLineBlack(sensorValues);
  int error = position - 3500;

  integral += error;
  int derivative = error - lastError;
  int motorSpeedDifference = (Kp * error) + (Ki * integral) + (Kd * derivative);
  lastError = error;

  int leftSpeed = baseSpeed + motorSpeedDifference;
  int rightSpeed = baseSpeed - motorSpeedDifference;

  setMotorSpeeds(leftSpeed, rightSpeed);

  // Affichage en temps réel pour vérifier que le programme tourne
  Serial.print("Pos: ");
  Serial.print(position);
  Serial.print(" | Err: ");
  Serial.print(error);
  Serial.print(" | V_Gauché: ");
  Serial.print(constrain(leftSpeed, -maxSpeed, maxSpeed));
  Serial.print(" | V_Droit: ");
  Serial.println(constrain(rightSpeed, -maxSpeed, maxSpeed));

  delay(50);
}
