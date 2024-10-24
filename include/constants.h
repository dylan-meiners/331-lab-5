#pragma once
#include <math.h>

constexpr const double ROTOR_RADIUS = 0.05;  // m
constexpr const double ROTOR_MASS = 0.65;    // kg
constexpr const double ROTOR_MASS_MOMENT_INERTIA =
    1.0 / 2.0 * ROTOR_MASS * ROTOR_RADIUS * ROTOR_RADIUS;
constexpr const double RW_MAX_ACCEL_RPM = 50;  // RPM
constexpr const double RW_MAX_ACCEL_RAD_S_S =
    RW_MAX_ACCEL_RPM * (2 * M_PI) / 60;
constexpr const double RW_TORQUE =
    ROTOR_MASS_MOMENT_INERTIA * RW_MAX_ACCEL_RAD_S_S;  // N.m

constexpr const double SLOW_SLEW_OUTPUT = 0.15;
constexpr const double SLOW_SLEW_DEADZONE_CLOSE = 5;  // degrees
constexpr const double SLOW_SLEW_DEADZONE_FAR = 10;   // degrees