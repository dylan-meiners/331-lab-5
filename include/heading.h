#pragma once
#include <Adafruit_BNO055.h>
#include <SPI.h>

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28, &Wire);
double maxX = 18.5;
double minX = 16.56;
double maxY = 15.69;
double minY = -12.38;
double offsetX;
double offsetY;

void setupBNO() {
  if (!bno.begin()) {
    Serial.println("Could not start bno!");
    while (true);
  }

  bno.setExtCrystalUse(true);

  long startTime = millis();
  Serial.println("Getting heading and not recording for 5 seconds...");
  while (millis() - startTime <= 5000) {
    auto _ = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  }

  Serial.println("BNO055 setup complete");
}

// Gathers the max and min values of each mag axis
void gatherMagDataLoop() {
  imu::Vector<3> mags = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  double x = mags.x();
  double y = mags.y();

  if (x > maxX) {
    maxX = x;
  }
  if (x < minX) {
    minX = x;
  }

  if (y > maxY) {
    maxY = y;
  }
  if (y < minY) {
    minY = y;
  }
}

// BE CAREFUL! Assumes offsets have been found
double GetHeading() {
  imu::Vector<3> mags = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  double x = mags.x();
  double y = mags.y();

  x -= offsetX;
  y -= offsetY;

  // Scale to 180 to -180
  x *= 180.0 / maxX;
  y *= 180.0 / maxY;

  double original = atan2(y, x) * 180 / M_PI;

  return original > 0 ? original : original + 360.0;
}