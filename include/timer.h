#pragma once
#include <Arduino.h>

bool reset = false;
long duration;
long startTime;

void resetTimer() { reset = true; }
bool waitDuration(long duration) {
  if (reset) {
    Serial.print("Starting timer for ");
    Serial.print(duration);
    Serial.println(" ms");
    startTime = millis();
    reset = false;
  }
  return (long)(millis() - startTime) >= duration;
}