#pragma once
#include <TB9051FTGMotorCarrier.h>

#include "pins.h"

TB9051FTGMotorCarrier driver(PWM_PIN_1, PWM_PIN_2);
float outVal = 0;

void setupRW() {
  driver.setBrakeMode(false);
  driver.enable();
  driver.setOutput(0);
  logMessageEverywhere("RW setup complete");
}

void setRW(float out) {
  outVal = out;
  driver.setOutput(-outVal);
}

void pulseRW() {
  setRW(1);
  delay(100);
  setRW(-1);
  delay(100);
  setRW(0);
}