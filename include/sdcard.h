#pragma once
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

File logFile;

void setupSD() {
  if (!SD.begin(SDCARD_SS_PIN)) {
    Serial.println("Could not open SD");
  } else {
    Serial.println("SD started");
  }
  logFile = SD.open("sp.txt", FILE_WRITE);
  Serial.print("logFile handle: ");
  Serial.println(logFile);
}

void writeSD(String str) {
  logFile.print(str);
  logFile.flush();
}