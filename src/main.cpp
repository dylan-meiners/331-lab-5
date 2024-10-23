#include <Arduino.h>
#include <QuadratureEncoder.h>

#include "constants.h"
#include "heading.h"
#include "pins.h"
#include "rw.h"
#include "slew.h"
#include "timer.h"

#define CALIBRATE_ONLY

// State
int state = 0;

// 0 = gather calibration data
#define CALIBRATE_TIME_S 30

// 1 = apply offsets

// 2 = slew to heading slow
#define TRY_SLEW_HEADING_SLOW_TIME_S 15

// 2 = slew to heading fast
#define TRY_SLEW_HEADING_TIME_S 10
double slewHeading;

// 3 = wait
#define WAIT_FOR_NEXT_SLEW_TIME_S 10

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
        printf("Switching states; going to 1\n");
        state = 1;
        break;
      }
      gatherMagDataLoop();
      printf(
          "Mag max/mins are: Min X = %f  Max X = %f  Min Y = %f  Max Y = %f\n",
          minX, maxX, minY, maxY);
      break;
    }
    case 1: {
      double xSpread = maxX - minX;
      double ySpread = maxY - minY;
      double offsetX = maxX - xSpread / 2;
      double offsetY = maxY - ySpread / 2;
      printf("Mag offsets are: X Offset = %f  Y Offset = %f\n", offsetX,
             offsetY);

#ifdef CALIBRATE_ONLY
      printf("Switching states; going to 5\n");
      state = 5;
#else
      printf("Switching states; going to 3\n");
      slewHeading = 0;
      resetTimer();
      state = 3;
#endif
      break;
    }
    case 2: {
      if (waitDuration(TRY_SLEW_HEADING_SLOW_TIME_S * 1000)) {
        printf("Switching states; going to 3\n");
        state = 3;
        resetTimer();
        break;
      }
      slewToHeadingSlow(0);
      break;
    }
    case 3: {
      slewToHeadingFast(slewHeading);
      if (waitDuration(TRY_SLEW_HEADING_TIME_S * 1000)) {
        printf("Switching states; going to 4\n");
        state = 4;
        resetTimer();
        break;
      }
      break;
    }
    case 4: {
      if (waitDuration(WAIT_FOR_NEXT_SLEW_TIME_S * 1000)) {
        printf("Switching states; going to %d\n", 2);
        state = 3;
        if (slewHeading == 0) {
          slewHeading = 180;
        } else {
          slewHeading = 0;
        }
        resetTimer();
        break;
      }
      break;
    }
    case 5: {
      double heading = GetHeading();
      printf("Heading: %f deg\n", heading);
      break;
    }
    default:
      printf("Defaulting state switch; state is %d\n", state);
      break;
  }
}