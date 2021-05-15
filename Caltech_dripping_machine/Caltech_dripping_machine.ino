#include "Positional.h"
#include "tray.h"
#include "calibration.h"

struct __attribute__((__packed__)) startData{
  byte START_MODE; //start mode can be 0 or 1 respectively normal run and calibration
  uint32_t mills; //ms/w
  byte end_well_index;
  byte tray_index;
  tray trayData;
};
union startDataArray{
  startData s;
  byte b[sizeof(startData)];
};

//term code, trigger to halt well loop and reset arduino
#define TERM_CODE 0x55
bool manual = false;
/**
 * By Riley Ramirez and Forrest Ramirez Oct 16 2020
 * For Caltech
 */
void setup() { 
  calibrator calibrationLoader;

  PositionalController* posControl = new PositionalController(calibrationLoader.getCalibrationValues());
  posControl->home();
  
  Serial.begin(19200);
  //wait for serial to send bytes
  while(!Serial.available());
  delay(1);
  startDataArray data;
  Serial.readBytes(data.b, sizeof(startData)); 
  
  Serial.write(1);
  trayProcessor processor(data.s.trayData.x, data.s.trayData.y, calibrationLoader.getCalibrationValues());
  processor.setEndIndex(data.s.end_well_index);
  processor.start(posControl);
  processor.setMillsPerWell(data.s.mills);
  processor.process(posControl);
  //reset after processing is done.
  delete posControl;
  asm volatile ("jmp 0");
}
void loop() {
  //reset, as shouldn't be here
  asm volatile ("jmp 0");
}
