#include <Arduino.h>
#include <QuadratureEncoder.h>

#include "constants.h"
#include "heading.h"
#include "log.h"
#include "pins.h"
#include "rw.h"
#include "sdcard.h"
#include "slew.h"
#include "timer.h"

// #define CALIBRATE_ONLY
// #define USE_PREDETERMINED_OFFSETS
// #define SLEW_SLOW_ONLY
// #define PULSE

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
double slewHeading = 180;
bool stopped = false;

// 4 = kill motor and wait
#define WAIT_TIME_S 5

// 5 = output heading

// 6 = pulse motor
#define PULSE_TIME 0.5
bool pulsing = false;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  setupSD();

  logMessageEverywhere("SARGE");

  setupRW();
  setupBNO();

  resetTimer();
}

long logTimer = 0;

void loop() {
  long now = millis();
  if (now - logTimer >= 100) {
    logTimer = now;
    String logStr = String(GetHeading()) + String(",") + String(outVal);
    logMessageEverywhere(logStr);
  }

  switch (state) {
    case 0: {
      if (waitDuration(CALIBRATE_TIME_S * 1000)) {
        logMessageEverywhere("Switching states; going to 1");
        pulseRW();
        delay(5000);
        state = 1;
        break;
      }
      gatherMagDataLoop();
      String str = "Mag max/mins are: Min X = ";
      str += minX;
      str += "  Max X = ";
      str += maxX;
      str += "  Min Y = ";
      str += minY;
      str += "  Max Y = ";
      str += maxY;
      str += "";
      logMessageEverywhere(str);
      break;
    }
    case 1: {
      double xSpread = maxX - minX;
      double ySpread = maxY - minY;
      double offsetX = maxX - xSpread / 2;
      double offsetY = maxY - ySpread / 2;
      String str = "";
      str += "Mag offsets are: X Offset = ";
      str += offsetX;
      str += "  Y Offset = ";
      str += offsetY;
      str += "";
      logMessageEverywhere(str);

#ifdef CALIBRATE_ONLY
      logMessageEverywhere("Switching states; going to 5");
      delay(5000);
      state = 5;
      break;
#endif
#ifdef PULSE
      logMessageEverywhere("Switching states; going to 6");
      delay(5000);
      state = 6;
      break;
#endif
      logMessageEverywhere("Switching states; going to 2");
      resetTimer();
      state = 2;
      break;
    }
    case 2: {
      if (waitDuration(TRY_SLEW_HEADING_SLOW_TIME_S * 1000)) {
        stopped = false;

#ifdef SLEW_SLOW_ONLY
        state = 2;
#else
        state = 3;
#endif
        resetTimer();
        break;
      }
      slewToHeadingSlow(0);
      break;
    }
    case 3: {
      if (waitDuration(TRY_SLEW_HEADING_TIME_S * 1000)) {
        // logMessageEverywhere("Switching states; going to 4");
        // state = 4;
        // pulseRW();
        // resetTimer();
        // break;
        logMessageEverywhere("Switching states; going to 5");
        state = 5;
        break;
      }
      if (!stopped) {
        slewToHeadingFast(slewHeading, &stopped);
      }
      break;
    }
    case 4: {
      if (waitDuration(WAIT_TIME_S * 1000)) {
        state = 2;
        resetTimer();
        break;
      }
      setRW(0);
      break;
    }
    case 5: {
      break;
    }
    case 6: {
      if (waitDuration(PULSE_TIME * 1000)) {
        pulsing = !pulsing;
        resetTimer();
      }
      setRW(pulsing ? -1 : 0);
      break;
    }
    default:
      String str = String("Defaulting state switch; state is ");
      str += state;
      logMessageEverywhere(str);
      break;
  }
}