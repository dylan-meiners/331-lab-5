#pragma once
#include "constants.h"
#include "heading.h"
#include "rw.h"

float rwSetpoint = 0;

void slewToHeadingSlow(double targetHeading) {
  // Output a relatively low power to the motor and wait until we're at the
  // right angle to stop. Does not know how to go the short way passing 360 ->
  // 0.
  double delta = GetHeading() - targetHeading;
  if (delta > 180) {
    delta = -(360 - delta);
  }
  if (abs(delta) < SLOW_SLEW_DEADZONE_CLOSE) {
    setRW(0);
  } else {
    double mag;
    if (abs(delta) < SLOW_SLEW_DEADZONE_FAR) {
      mag = 0.1;
    } else {
      mag = 0.2;
    }

    int sign = delta / abs(delta);
    double output = mag * sign;
    setRW(output);
  }
}

void slewToHeadingFast(double targetHeading, bool* stopped) {
  // Note: only works with headings of 0 and 180 deg, and assumes we are
  // starting from the other heading

  // To slew in the least amount of time, assume the reaction wheel will
  // accelerate while spinning up and slowing down at the same magnitude. This
  // allows for us to use the equation relating T_slew and time. We also assume
  // that the reaction wheel does not reach maximum angular velocity at any
  // point during the maneuver.

  double currentHeading = GetHeading();

  // If not halfway there yet
  // The angle could be from a little less than zero; assume worst case it is
  // 300 deg
  if (currentHeading <= (targetHeading / 2.0) || currentHeading >= 300) {
    setRW(-1);  // TODO: This may change in practice
  }
  // If over half way there, stop the motor
  else {
    setRW(0);
    *stopped = true;
  }
}

void slewToHeadingCutoff(double targetHeading) {}