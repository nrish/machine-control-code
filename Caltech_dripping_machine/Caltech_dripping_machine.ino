#include "Positional.h"
#include "tray.h"
#include "calibration.h"

//term code, trigger to halt well loop and reset arduino
#define TERM_CODE 0x55
/**
   By Riley Ramirez and Forrest Ramirez Oct 16 2020
   For Caltech
*/
bool expflag = true;
expectSerialized expectedBytes;
int buffsize = 0;
byte buff[100];
calibrator calibrationLoader;
PositionalController* posControl;
void readFromBuff(uint8_t* dest, int size){
  for(int i = 0; i < size && i < 100; i++){
    dest[i] = buff[i];
  }
  buffsize = 0;
}
void setup() {

  Serial.begin(19200);
  //initalize calibration profile
  calibrationLoader.loadEEPROM();

  posControl = new PositionalController(calibrationLoader.getCalibrationValues());
}
void serialEvent() {
  if (Serial.available() >= sizeof(expect) && expflag) {
    Serial.readBytes((char*)expectedBytes.bytes, sizeof(expect));
    //got exp command
    //get cmd
    buffsize = 0;
    while(buffsize < expectedBytes.values.bytes && !expectedBytes.values.request){
      int temp = Serial.available();
      Serial.readBytes((char*)(&buff+buffsize), temp+buffsize > 100 ? 100-buffsize : temp);
      buffsize += temp > 100 ? 100 : temp;
    }
    expflag = false;
  }
}
void loop() {
  if (!expflag && (buffsize >= expectedBytes.values.bytes || expectedBytes.values.request)) {
    if (expectedBytes.values.cmd == CMD_STARTDATA) {
      //expect start data
      StartDataSerialized startDataSerialized;
      readFromBuff(startDataSerialized.bytes, sizeof(StartData));
      uint8_t trayIndex = startDataSerialized.values.trayIndex;
      trayProcessor processor(calibrationLoader.getCalibrationValues()->trays[trayIndex].x, calibrationLoader.getCalibrationValues()->trays[trayIndex].y, calibrationLoader.getCalibrationValues());
      processor.setEndIndex(startDataSerialized.values.endWellIndex);
      processor.start(posControl);
      processor.setMillsPerWell(startDataSerialized.values.mills);
      processor.process(posControl);
      posControl->home();
    } else if (expectedBytes.values.cmd == CMD_CALIBRATION) {
      //expect calibration data
      if (expectedBytes.values.request) {
        CalibrationValueSerialized profile;
        profile.values = calibrationLoader.copyCalibrationValues();
        expectSerialized reply = expect(CMD_CALIBRATION, false);
        Serial.write(reply.bytes, sizeof(expect));
        Serial.write(profile.bytes, sizeof(CalibrationValues));
        Serial.flush();
      } else {
        //write cmd
        CalibrationValueSerialized profile;
        readFromBuff(profile.bytes, sizeof(CalibrationValues));
        Serial.println("Read profile");
        Serial.flush();
        calibrationLoader.setCalibrationValues(profile.values);
        calibrationLoader.saveToEEPROM();
        expectSerialized reply = expect(CMD_UPDATE, false);
        updateDataSerialized up = updateData(false, false, false, true);
        Serial.write(reply.bytes, sizeof(expect));
        Serial.write(profile.bytes, sizeof(CalibrationValues));
        Serial.flush();
        
      }
      
    } else if (expectedBytes.values.cmd == CMD_UPDATE) {
      //nothing to do, invalid command. Just swollow it.

    } else if (expectedBytes.values.cmd == CMD_SETPOS) {
      while (Serial.available() < expectedBytes.values.bytes);
      setPosSerialized posData;
      readFromBuff(posData.bytes, sizeof(posData));
      if (posData.values.home) {
        posControl->setFastMode(true);
        posControl->home();
        posControl->setFastMode(true);
      } else {
        posControl->setFastMode(posData.values.speed);
        posControl->setPos(posData.values.x, posData.values.y);
      }

    }else{
    }
    Serial.flush();
    expflag = true;
  }
}
