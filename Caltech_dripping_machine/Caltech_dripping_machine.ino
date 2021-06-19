#include "Positional.h"
#include "tray.h"
#include "calibration.h"
#include "SerialTools.h"
//term code, trigger to halt well loop and reset arduino
#define TERM_CODE 0x55
/**
   By Riley Ramirez and Forrest Ramirez Oct 16 2020
   For Caltech
*/
bool expflag = false;
expectSerialized expectedCommand;
int buffsize = 0;
calibrator calibrationLoader;
PositionalController* posControl;

void setup() {

  Serial.begin(19200);
  //initalize calibration profile
  calibrationLoader.loadEEPROM();

  posControl = new PositionalController(calibrationLoader.getCalibrationValues());
  analogWrite(pins::PWM_SERVO, 160);
  posControl->home();
}
void serialEvent() {
  readExpect(expflag, expectedCommand);
}
void loop() {
  if (expflag) {
    if(!expectedCommand.values.request)
      readPacketStream(expectedCommand.values.bytes);
    if (expectedCommand.values.cmd == CMD_STARTDATA && !expectedCommand.values.request) {
      //expect start data
      StartDataSerialized startDataSerialized;
      memcpy(startDataSerialized.bytes, buff, sizeof(StartDataSerialized));
      uint8_t trayIndex = startDataSerialized.values.trayIndex;
      trayProcessor processor(calibrationLoader.getCalibrationValues()->trays[trayIndex].x, calibrationLoader.getCalibrationValues()->trays[trayIndex].y, calibrationLoader.copyCalibrationValues());
      processor.setEndIndex(startDataSerialized.values.endWellIndex);
      processor.start(posControl);
      processor.setMillsPerWell(startDataSerialized.values.mills);
      processor.process(posControl);
      posControl->home();
    } else if (expectedCommand.values.cmd == CMD_CALIBRATION) {
      //expect calibration data
      if (expectedCommand.values.request) {
        CalibrationValueSerialized profile;
        profile.values = calibrationLoader.copyCalibrationValues();
        
        sendExpect(expect(CMD_CALIBRATION, false));
        writePacketStream(profile.bytes, sizeof(CalibrationValues));
      } else {
        //write cmd
        CalibrationValueSerialized profile;
        memcpy(profile.bytes, buff, sizeof(CalibrationValueSerialized));
        calibrationLoader.setCalibrationValues(profile.values);
        calibrationLoader.saveToEEPROM();
        updateDataSerialized up = updateData(false, false, false, true);
        
        sendExpect(expect(CMD_UPDATE, false));
        writePacketStream(up.bytes, sizeof(updateData)); 
      }
      
    } else if (expectedCommand.values.cmd == CMD_UPDATE) {
      //nothing to do, invalid command. Just swollow it.

    } else if (expectedCommand.values.cmd == CMD_SETPOS && !expectedCommand.values.request) {
      setPosSerialized posData;
      memcpy(posData.bytes, buff, sizeof(setPosSerialized));
      if (posData.values.home) {
        posControl->setFastMode(true);
        posControl->home();
        posControl->setFastMode(false);
      } else {
        posControl->setPos(posData.values.x, posData.values.y);
      }

    }else{
    }
    Serial.flush();
    expflag = false;
  }
}
