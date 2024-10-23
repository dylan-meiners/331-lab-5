#pragma once
#include <Adafruit_BNO055.h>
#include <SPI.h>

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28, &Wire);
double maxX;
double minX;
double maxY;
double minY;
double offsetX;
double offsetY;

void setupBNO() {
  if (!bno.begin()) {
    Serial.println("Could not start bno!");
    while (true);
  }

  bno.setExtCrystalUse(true);
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

  return atan2(y, x);
}