#ifndef TRAY_H
#define TRAY_H
#include "Stepper.h"
#include "Positional.h"
#include "calibration.h"
// pull has diameter of 9.6, 9.6/2 = 4.8, 2pi * 4.8 = 30.16, 1600/30.16 (1/8 stepping) = 53.05.
// well distance is about 9mm, 9mm * 53 steps/mm = 477.45 steps

//probably doesn't need to be calibratable
const int ROWS = 8;
const int COLS = 12;
class trayProcessor {
  private:
    int x;
    int y;
    int endIndex = 0;
    uint32_t time = 0;
    boolean dir = false;
    CalibrationValues values;
  public:
    trayProcessor(int x, int y, CalibrationValues values) {
      this->y = y;
      this->x = x;
      this->values = values;
    }

    int getIndex(int col, int row) {
      return col + row * 12;
    }
    int getRow(int index) {
      return index / 12;
    }
    int getCol(int index) {
      return index % 12;
    }
    void setMillsPerWell(uint32_t time) {
      this->time = time;
    }
    void start(PositionalController* pos) {
      analogWrite(pins::PWM_SERVO, 160);
      delay(150);
      analogWrite(pins::PWM_SERVO, 0);
      pos->setFastMode(true);
      pos->setPos(x, y);
      pos->setFastMode(true);
    }
    void process(PositionalController* pos) {
      for (int i = 0; i < ROWS; i++) {
        for (int l = 0; l < COLS; l++) {
          //send update expect

          if (endIndex == getIndex(l, i))
            return;
          float servopos = 160;
          long startTime = millis();
          while (millis() - startTime < time) {
            if (servopos > 90) {
              servopos -= 0.5;
              analogWrite(pins::PWM_SERVO, (int)servopos);
            }
            delay(1);

          }
          analogWrite(pins::PWM_SERVO, 160);

          if (l + 1 != COLS) {
            if (i % 2) {
              pos->steps(-values.WELL_DIST_X, 0);
            } else {
              pos->steps(values.WELL_DIST_X, 0);
            }
          } 
        }
        pos->steps(0, values.WELL_DIST_Y);
      }
      pos->home();
      analogWrite(pins::PWM_SERVO, 0);
    }
    void setEndIndex(int end) {
      endIndex = end;
    }
};
#endif
