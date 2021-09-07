#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <EEPROM.h>
#define EEPROM_OFFSET 0
struct __attribute__((__packed__)) Tray{
  int16_t x;
  int16_t y;
};
struct __attribute__((__packed__)) CalibrationValues{
    CalibrationValues(){}
    //missing constructor as it shouldn't ever need initalization.
    //distance between wells
    int16_t WELL_DIST_X;
    int16_t WELL_DIST_Y;

    bool Y_END_DIR;
    bool X_END_DIR;
    //positions of the 8 trays on machine
    Tray trays[8];
};

class calibrator{
  private:
  CalibrationValues values;
  
  public:
  calibrator(){
    //just load calibration into values.
    loadEEPROM();
  }
  
  void loadEEPROM(){
    CalibrationValues data;
    byte* ptr = (byte*)&data;
    for(byte i = 0; i < sizeof(CalibrationValues); i++){
      ptr[i] = EEPROM.read(EEPROM_OFFSET+i);
    }
    this->values = data;
  }
  
  void saveToEEPROM(){
    byte* ptr = (byte*)&values;
    for(byte i = 0; i < sizeof(CalibrationValues); i++){
      //to preserve the EEPROM, check if this value is already stored.
      //right now to keep the current calibration profiles safe we disable writing.
      EEPROM.update(EEPROM_OFFSET+i, ptr[i]);
    }
  }
  CalibrationValues* getValues(){
    return &values;
  }
  void setCalibrationValues(CalibrationValues& values){
    this->values = values;
  }
};

namespace pins{
  int X_STEP = 2;
  int Y_STEP = 3;
  int X_DIR = 5;
  int Y_DIR = 6;
  int Y_END = 10; //pulled low when on
  int X_END = 9;  //pulled low when on
  int PWM_SERVO = 11;
  int STEPPER_ENABLE = 8; //pull low
};
#endif
