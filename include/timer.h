#pragma once
#include <Arduino.h>

bool reset = false;
long duration;
long startTime;

void resetTimer() { reset = true; }
bool waitDuration(long duration) {
  if (reset) {
    String str =
        String("Starting timer for ") + String(duration) + String(" ms");
    logMessageEverywhere(str);
    startTime = millis();
    reset = false;
  }
  return (long)(millis() - startTime) >= duration;
}