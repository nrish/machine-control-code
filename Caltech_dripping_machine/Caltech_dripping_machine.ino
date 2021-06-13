#include "Positional.h"
#include "tray.h"
#include "calibration.h"

//term code, trigger to halt well loop and reset arduino
#define TERM_CODE 0x55
/**
   By Riley Ramirez and Forrest Ramirez Oct 16 2020
   For Caltech
*/
void setup() {

  Serial.begin(19200);
  //initalize calibration profile
  calibrator calibrationLoader;
  calibrationLoader.loadEEPROM();

  PositionalController* posControl = new PositionalController(calibrationLoader.getCalibrationValues());
  //wait for serial to send bytes
  while (Serial.available() != sizeof(StartData));
  //serial mode
  expectSerialized expectedBytes;
  while (true) {
    //wait till there is enough bytes to get command spec
    while (Serial.available() < sizeof(expect));
    Serial.readBytes(expectedBytes.bytes, sizeof(expect));
    if (expectedBytes.value.cmd == CMD_STARTDATA) {
      //expect start data
      while (Serial.available() < expectedBytes.value.bytes);

      StartDataSerialized startDataSerialized;
      Serial.readBytes(startDataSerialized.bytes, sizeof(StartData));
      uint8_t trayIndex = startDataSerialized.values.trayIndex;
      trayProcessor processor(calibrationLoader.getCalibrationValues()->trays[trayIndex].x, calibrationLoader.getCalibrationValues()->trays[trayIndex].y, calibrationLoader.getCalibrationValues());
      processor.setEndIndex(startDataSerialized.values.endWellIndex);
      processor.start(posControl);
      processor.setMillsPerWell(startDataSerialized.values.mills);
      processor.process(posControl);
      posControl->home();
    } else if (expectedBytes.value.cmd == CMD_CALIBRATION) {
      //expect calibration data
      if (expectedBytes.value.request) {
        CalibrationValueSerialized profile;
        profile.values = calibrationLoader.copyCalibrationValues();
        Serial.write(profile.bytes, sizeof(CalibrationValues));
      } else {
        //write cmd
        CalibrationValueSerialized profile;
        while (Serial.available() < expectedBytes.value.bytes);
        Serial.readBytes(profile.bytes, sizeof(CalibrationValues));
        calibrationLoader.setCalibrationValues(profile.values);
        calibrationLoader.saveToEEPROM();
      }
    } else if (expectedBytes.value.cmd == CMD_UPDATE) {
      //nothing to do, invalid command. Just swollow it.

    } else if (expectedBytes.value.cmd == CMD_SETPOS) {
      while (Serial.available() < expectedBytes.value.bytes);
      setPosSerialized posData;
      Serial.readBytes(posData.bytes, sizeof(posData));
      if (posData.values.home) {
        posControl->setFastMode(true);
        posControl->home();
        posControl->setFastMode(true);
      } else {
        posControl->setFastMode(posData.values.speed);
        posControl->setPos(posData.values.x, posData.values.y);
      }

    } else {

    }
  }

}
void loop() {
  //reset, as shouldn't be here
  asm volatile ("jmp 0");
}
