#ifndef CALIBRATION_H
#define CALIBRATION_H
#include <EEPROM.h>


struct __attribute__((__packed__)) tray{
  int x; // index 0
  int y; // index 1
};

union writedata{
  float f;
  byte b[4];
  tray t;
  uint32_t l;
};

struct __attribute__((__packed__)) calibrationValues{
  byte WELL_DIST_X;
  byte WELL_DIST_Y;
  
  byte TRAY_DIST_X;
  byte TRAY_DIST_Y;

  bool Y_END_DIR;
  bool X_END_DIR;

  tray trays[8];
};
union EEPROMData{
  byte b[sizeof(calibrationValues)];
  calibrationValues v;
};

class calibrator{
  private:
  calibrationValues values;
  
  public:
  calibrator(){
    //just load calibration into values.
    loadEEPROM();
  }
  
  void loadEEPROM(){
    EEPROMData data;
    for(byte i = 0; i < sizeof(values); i++){
      data.b[i] = EEPROM.read(i);
    }
    this->values = data.v;
    
  }
  
  void saveEEPROM(){
    EEPROMData data;
    data.v = this->values;
    for(byte i = 0; i < sizeof(values); i++){
      //to preserve the EEPROM, check if this value is already stored.
      EEPROM.update(i, data.b[i]);
    }
  }
  byte* toByteArray(){
    
  }
  calibrationValues* getCalibrationValues(){
    return &values;
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
