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
  
  Serial.begin(19200);
  calibrator calibrationLoader;
  PositionalController* posControl = new PositionalController(calibrationLoader.getCalibrationValues());
  //wait for serial to send bytes
  while(Serial.available() != sizeof(StartData));
  delay(1);
  StartDataSerialized startDataSerialized;
  Serial.readBytes(startDataSerialized.bytes, sizeof(StartData)); 
  //write all 1s if successful
  int trayIndex = startDataSerialized.startData.trayIndex;
  if(startDataSerialized.startData.start_mode == 0){  
    trayProcessor processor(calibrationLoader.getCalibrationValues()->trays[trayIndex].x, calibrationLoader.getCalibrationValues()->trays[trayIndex].y, calibrationLoader.getCalibrationValues());
    processor.setEndIndex(startDataSerialized.startData.endWellIndex);
    processor.start(posControl);
    processor.setMillsPerWell(startDataSerialized.startData.mills);
    processor.process(posControl);
  }else{
    //here we accept calibration profiles from serial
    EEPROMData profile;
    profile.values = *calibrationLoader.getCalibrationValues();
    Serial.write(profile.bytes, sizeof(CalibrationValues));
    while(Serial.available() != sizeof(CalibrationValues)) delay(1);
    Serial.readBytes(profile.bytes, sizeof(CalibrationValues));
    calibrationLoader.saveToEEPROM();
  }
  posControl->home();
  //reset after processing is done.
  delete posControl;
  asm volatile ("jmp 0");
}
void loop() {
  //reset, as shouldn't be here
  asm volatile ("jmp 0");
}
