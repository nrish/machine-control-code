#ifndef TRAY_H
#define TRAY_H
#include "Stepper.h"
#include "Positional.h"
// pull has diameter of 9.6, 9.6/2 = 4.8, 2pi * 4.8 = 30.16, 1600/30.16 (1/8 stepping) = 53.05.
// well distance is about 9mm, 9mm * 53 steps/mm = 477.45 steps
const int WELL_DIST_ROW = 452; //steps for well distance
const int WELL_DIST_COL = 452;
const int ROWS = 8;
const int COLS = 12;
class tray {
  private:
    int x;
    int y;
    int endIndex = 0;
    boolean dir = false;
  public:
    tray(int x, int y) {
      this->y = y;
      this->x = x;
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
    void start(PositionalController* pos) {
      pos->setPos(x,y);
      analogWrite(pins::PWM_SERVO, 160);
      delay(150);
      analogWrite(pins::PWM_SERVO, 0);
    }
    void process(PositionalController* pos, unsigned long time) {
      for (int i = 0; i < ROWS; i++) {
        for (int l = 0; l < COLS; l++) {
          if(Serial.available()){
            byte a = Serial.read();
            if(a == 0x03){
              return;
            }
          }
          if(endIndex == getIndex(l,i))
            return;
          float servopos = 160;
          long startTime = millis();
          while (millis() - startTime < time) {
            if (servopos > 90){
              servopos -= 0.5;
              analogWrite(pins::PWM_SERVO, (int)servopos);
            }
            delay(1);
            
          }
          analogWrite(pins::PWM_SERVO, 160);

          if(l+1 != COLS){
            if(i % 2){
              pos->steps(-WELL_DIST_COL, 0);
            }else{
              pos->steps(WELL_DIST_COL, 0);
            }
          }
          Serial.write(0x01);
          
        }
        pos->steps(0, WELL_DIST_ROW); 
      }
      pos->home();
      Serial.write(0x02);
      analogWrite(pins::PWM_SERVO, 0);
    }
    void setEndIndex(int end){
      endIndex = end;
    }
};
#endif
