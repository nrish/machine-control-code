  #ifndef EVENTS_H
#define EVENTS_H
#include "Positional.h"
#include "calibration.h"
#include "tray.h"
typedef void (*evt_function)(char* data, PositionalController* controller, calibrator* cal);
#define evtFunc(name) void evt_##name(char* data, PositionalController* controller, calibrator* cal)
#define evt(name) evt_##name

const unsigned char id_debugmsg = 52;
const unsigned char id_calibrationDataResponse = 51;
//all data is assumed to be in little endian (native to arduino)

trayProcessor trays[8];
evtFunc(printTestInfo){
  Serial.println("Dripper v8.2\nBy Forrest Ramirez & Riley Ramirez\nrramirez6821@gmail.com");
}
evtFunc(setPos){
  uint16_t * ptr = (uint16_t*)data;
  uint16_t x = ptr[0];
  uint16_t y = ptr[1];
  controller->setPos(x,y);
};
evtFunc(home){
  controller->home();
};
evtFunc(addTray){
  //format: index, time, start, end
  byte index = data[0];
  trays[index].index = index;
  trays[index].time = *((uint16_t*)data+1);
  trays[index].startIndex = data[3];
  trays[index].endIndex = data[4];
  trays[index].enabled = true;
  
};
evtFunc(removeTray){
  byte index = data[0];
  trays[index].enabled = false;
};
evtFunc(clearTrays){
  for(int i = 0; i < 8; i++){
    trays[i].enabled = false;
  }
};
evtFunc(start){
  for(int i = 0; i < 8; i++){
    if(trays[i].enabled){
      trays[i].start(controller, cal->getValues());
      trays[i].process(controller, cal->getValues());
    }
  }
};
evtFunc(setDistCalibration){
  uint16_t* ptr = (uint16_t*)data;
  cal->getValues()->WELL_DIST_X = ptr[0];
  cal->getValues()->WELL_DIST_Y = ptr[1];
  cal->getValues()->X_END_DIR = data[4];
  cal->getValues()->Y_END_DIR = data[5];  
};
evtFunc(setTrayCalibration){
  byte index = data[0];
  uint16_t* ptr = (uint16_t*)(data+1); 
  cal->getValues()->trays[index].x = ptr[0];
  cal->getValues()->trays[index].y = ptr[1];
  
}

evtFunc(confirmCalibration){
  cal->saveToEEPROM();
};
evtFunc(getCalibrationData){
  Serial.write(38);
  Serial.write(id_calibrationDataResponse);
  Serial.write((byte*)&cal->getValues()->WELL_DIST_X, 2);
  Serial.write((byte*)&cal->getValues()->WELL_DIST_Y, 2);
  Serial.write((byte*)&cal->getValues()->X_END_DIR, 1);
  Serial.write((byte*)&cal->getValues()->Y_END_DIR, 1);
  for(int i = 0; i < 8; i++){
    Serial.write((byte*)&cal->getValues()->trays[i].x, 2);
    Serial.write((byte*)&cal->getValues()->trays[i].y, 2);
  }
  Serial.flush();
};
evt_function events[] = {
  &evt(printTestInfo),
  &evt(setPos),
  &evt(home),
  &evt(addTray),
  &evt(removeTray),
  &evt(clearTrays),
  &evt(start),
  &evt(setDistCalibration),
  &evt(setTrayCalibration),
  &evt(confirmCalibration),
  &evt(getCalibrationData)
};
#endif EVENTS_H
