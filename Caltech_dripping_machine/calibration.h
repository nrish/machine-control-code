#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <EEPROM.h>
#include "serialData/serialData.h"
#define EEPROM_OFFSET 0

class calibrator{
  private:
  CalibrationValues values;
  
  public:
  calibrator(){
    //just load calibration into values.
    loadEEPROM();
  }
  
  void loadEEPROM(){
    CalibrationValueSerialized data;
    for(byte i = 0; i < sizeof(values); i++){
      data.bytes[i] = EEPROM.read(EEPROM_OFFSET+i);
    }
    this->values = data.values;
    
  }
  
  void saveToEEPROM(){
    CalibrationValueSerialized data;
    data.values = this->values;
    for(byte i = 0; i < sizeof(values); i++){
      //to preserve the EEPROM, check if this value is already stored.
      EEPROM.update(EEPROM_OFFSET+i, data.bytes[i]);
    }
  }
  byte* toByteArray(){
    
  }
  CalibrationValues* getCalibrationValues(){
    return &values;
  }
  CalibrationValues copyCalibrationValues(){
    return values;
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
