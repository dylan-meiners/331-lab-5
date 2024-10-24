#include <Arduino.h>
#include <QuadratureEncoder.h>

#include "constants.h"
#include "heading.h"
#include "pins.h"
#include "rw.h"
#include "slew.h"
#include "timer.h"

// #define CALIBRATE_ONLY
// #define USE_PREDETERMINED_OFFSETS

// State
#ifdef USE_PREDETERMINED_OFFSETS
int state = 1;
#else
int state = 0;
#endif

// 0 = gather calibration data
#define CALIBRATE_TIME_S 10

// 1 = apply offsets

// 2 = slew to heading slow
#define TRY_SLEW_HEADING_SLOW_TIME_S 10

// 2 = slew to heading fast
#define TRY_SLEW_HEADING_TIME_S 3
double slewHeading = 0;

// 4 = output heading

void setup() {
  Serial.begin(115200);
  Serial.println("SARGE");

  setupRW();
  setupBNO();

  resetTimer();
}

void loop() {
  switch (state) {
    case 0: {
      if (waitDuration(CALIBRATE_TIME_S * 1000)) {
        Serial.println("Switching states; going to 1\n");
        setRW(1);
        delay(100);
        setRW(-1);
        delay(100);
        setRW(0);
        delay(5000);
        state = 1;
        break;
      }
      gatherMagDataLoop();
      Serial.print("Mag max/mins are: Min X = ");
      Serial.print(minX);
      Serial.print("  Max X = ");
      Serial.print(maxX);
      Serial.print("  Min Y = ");
      Serial.print(minY);
      Serial.print("  Max Y = ");
      Serial.println(maxY);
      break;
    }
    case 1: {
      double xSpread = maxX - minX;
      double ySpread = maxY - minY;
      double offsetX = maxX - xSpread / 2;
      double offsetY = maxY - ySpread / 2;
      Serial.print("Mag offsets are: X Offset = ");
      Serial.print(offsetX);
      Serial.print("  Y Offset = ");
      Serial.println(offsetY);

#ifdef CALIBRATE_ONLY
      Serial.println("Switching states; going to 5");
      delay(5000);
      state = 5;
#else
      Serial.println("Switching states; going to 2");
      resetTimer();
      state = 2;
#endif
      break;
    }
    case 2: {
      if (waitDuration(TRY_SLEW_HEADING_SLOW_TIME_S * 1000)) {
        Serial.println("Switching states; going to 3");
        if (slewHeading == 0) {
          slewHeading = 180;
        } else {
          slewHeading = 0;
        }
        state = 3;
        resetTimer();
        break;
      }
      slewToHeadingSlow(slewHeading);
      break;
    }
    case 3: {
      if (waitDuration(TRY_SLEW_HEADING_TIME_S * 1000)) {
        Serial.println("Switching states; going to 4");
        state = 2;
        resetTimer();
        break;
      }
      slewToHeadingFast(slewHeading);
      break;
    }
    case 5: {
      double heading = GetHeading();
      Serial.print("Heading: ");
      Serial.print(heading);
      Serial.println(" deg");
      break;
    }
    default:
      Serial.print("Defaulting state switch; state is ");
      Serial.println(state);
      break;
  }
}