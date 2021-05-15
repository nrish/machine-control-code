#ifndef TRAY_H
#define TRAY_H
#include "Stepper.h"
#include "Positional.h"
#include "calibration.h"
// pull has diameter of 9.6, 9.6/2 = 4.8, 2pi * 4.8 = 30.16, 1600/30.16 (1/8 stepping) = 53.05.
// well distance is about 9mm, 9mm * 53 steps/mm = 477.45 steps

//term code, trigger to halt well loop and reset arduino
#define TERM_CODE 0x55
#define UPDATE_CODE 0x02
#define DONE_CODE 0x01
//These constants are for the 96 well tray this machine is made for. These values should probably not be calibratable.
//DIST_ROW and DIST_COL are different values in case machine has problem with inexact steppers, but can be the same in theory.

const int ROWS = 8;
const int COLS = 12;
class trayProcessor {
  private:
    int x;
    int y;
    int endIndex = 0;
    uint32_t time = 0;
    boolean dir = false;
    calibrationValues* values;
  public:
    trayProcessor(int x, int y, calibrationValues* values) {
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
    void setMillsPerWell(uint32_t time){
      this->time = time;
    }
    void start(PositionalController* pos) {
      pos->setPos(x,y);
      analogWrite(pins::PWM_SERVO, 160);
      delay(150);
      analogWrite(pins::PWM_SERVO, 0);
    }
    void process(PositionalController* pos) {
      for (int i = 0; i < ROWS; i++) {
        for (int l = 0; l < COLS; l++) {
          //check if termination is requested.
          if(Serial.available()){
            byte a = Serial.read();
            if(a == TERM_CODE){
              return;
            }
          }
          
          //check if we have reached the final well.
          if(endIndex == getIndex(l,i))
            return;

          //a little messy, but slowly move servo instead of making it jump and keep track of time while doing this.
          float servopos = 160;
          long startTime = millis();
          while (millis() - startTime < this->time) {
            if (servopos > 90){
              servopos -= 0.5;
              analogWrite(pins::PWM_SERVO, (int)servopos);
            }
            //delay one ms, this might need to be removed if this turns out to cause too much of a time diff.
            delay(1);
            
          }
          analogWrite(pins::PWM_SERVO, 160);

          //check if the next well is the last one in col. if so go the next one
          if(l+1 != COLS){
            if(i % 2){
              pos->steps(-values->WELL_DIST_X, 0);
            }else{
              pos->steps(values->WELL_DIST_X, 0);
            }
          }else{
            //special behavior here if head hangs over well for too long, but testing seems to indicate this is ok.
          }
          Serial.write(UPDATE_CODE);
          
        }
        pos->steps(0, values->WELL_DIST_Y); 
      }
      pos->home();
      Serial.write(DONE_CODE);
      analogWrite(pins::PWM_SERVO, 0);
    }
    void setEndIndex(int end){
      endIndex = end;
    }
};
#endif
