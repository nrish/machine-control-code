#include "Positional.h"
#include "tray.h"
#include "calibration.h"

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
  StartDataSerialized startDataSerialized;
  Serial.readBytes(startDataSerialized.bytes, sizeof(StartData)); 
  //write all 1s if successful
  Serial.write(0xFF);
  if(startDataSerialized.StartData.start_mode == 0){  
    trayProcessor processor(startDataSerialized.StartData.trayData.x, startDataSerialized.StartData.trayData.y, calibrationLoader.getCalibrationValues());
    processor.setEndIndex(startDataSerialized.StartData.end_well_index);
    processor.start(posControl);
    processor.setMillsPerWell(startDataSerialized.StartData.mills);
    processor.process(posControl);
  }else{
    //here we accept calibration profiles from serial
    EEPROMData profile;
    profile.values = *calibrationLoader.getCalibrationValues();
    Serial.readBytes(profile.bytes, sizeof(CalibrationValues));
    calibrationLoader.saveToEEPROM();
  }
  //reset after processing is done.
  delete posControl;
  asm volatile ("jmp 0");
}
void loop() {
  //reset, as shouldn't be here
  asm volatile ("jmp 0");
}
