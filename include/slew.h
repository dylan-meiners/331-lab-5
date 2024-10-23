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
  if (abs(delta) < SLOW_SLEW_DEADZONE) {
    setRW(0);
  } else {
    int sign = delta / abs(delta);
    setRW(SLOW_SLEW_OUTPUT * sign);
  }
}

void slewToHeadingFast(double targetHeading) {
  // Note: only works with headings of 0 and 180 deg, and assumes we are
  // starting from the other heading

  // To slew in the least amount of time, assume the reaction wheel will
  // accelerate while spinning up and slowing down at the same magnitude. This
  // allows for us to use the equation relating T_slew and time. We also assume
  // that the reaction wheel does not reach maximum angular velocity at any
  // point during the maneuver. Use a simple state machine to control the flow.
  // This is the procedure:
  // 1. Determine which direction to spin the motor (state 0)
  // 2. Set the motor's output to max. This will result in maximum angular
  // acceleration. (state 1)
  // 3. Wait until we are half way from the initial heading to the final
  // heading. (state 1)
  // 4. Set the motor's output to zero. This will result in maximum angular
  // acceleration in the negative direction. (state 1)

  double currentHeading = GetHeading();

  if (targetHeading == 0) {
    // If not halfway there yet
    if (currentHeading >= 90) {
      setRW(1);  // TODO: This may change in practice
    }
    // If over half way there, stop the motor
    else {
      setRW(0);
    }
  }
  // If going to 180 degrees heading
  else {
    // If not halfway there yet
    // The angle could be from a little less than zero; assume worst case it is
    // 300 deg
    if (currentHeading <= 90 || currentHeading >= 300) {
      setRW(-1);  // TODO: This may change in practice
    }
    // If over half way there, stop the motor
    else {
      setRW(0);
    }
  }
}