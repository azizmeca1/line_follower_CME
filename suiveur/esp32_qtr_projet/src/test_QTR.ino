#include <Arduino.h>
#include <QTRSensors.h>

QTRSensors qtr;
const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
const uint8_t qtrPins[SensorCount] = {32, 33, 25, 26, 27, 14, 34, 13};

void setup() {
  Serial.begin(115200);

  qtr.setTypeAnalog();
  qtr.setSensorPins(qtrPins, SensorCount);

  Serial.println("==========================================");
  Serial.println(" CALIBRATION: Balayez la ligne noire ! ");
  Serial.println("==========================================");

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(10);
  }

  Serial.println("Calibration terminée ! Lecture en cours...");
  delay(1000);
}

void loop() {
  uint16_t position = qtr.readLineBlack(sensorValues);

  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print("C");
    Serial.print(i + 1);
    Serial.print(":");
    Serial.print(sensorValues[i]);
    Serial.print("\t");
  }

  Serial.print("| Pos: ");
  Serial.println(position);

  delay(200);
}
