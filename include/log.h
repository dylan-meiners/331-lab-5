#pragma once
#include <Arduino.h>

#include "sdcard.h"

void logMessageEverywhere(String str) {
  str = String(millis()) + String(",") + str + String("\r\n");
  Serial.print(str);
  Serial1.print(str);
  // writeSD(str);
}