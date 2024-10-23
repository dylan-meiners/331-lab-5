#pragma once
#include <TB9051FTGMotorCarrier.h>

#include "pins.h"

TB9051FTGMotorCarrier driver(PWM_PIN_1, PWM_PIN_2);

void setupRW() {
  driver.setBrakeMode(true);
  driver.enable();
  driver.setOutput(0);
}

void setRW(float out) { driver.setOutput(out); }