#include "Positional.h"
#include "tray.h"
#include "calibration.h"

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

/*
 * Experimental serial function
 */
uint8_t* buff = new uint8_t[128];
void writePacketStream(uint8_t* data, size_t len){
  //get number of packets needed to send data plus one if it doesn't fit evenly
  int packets = len/16 + len%16;
  memcpy(buff, data, len);
  for(int i = 0; i < packets; i++){
    //copy 16 byte chunks and write
    Serial.write((buff+i*16), 16);
    //wait for confirm bit
    while(Serial.available() < 0);
    //read single confirm bit
    Serial.read();
  }
}
void readPacketStream(size_t len){
  int packets = len/16 + len%16;
  for(int i = 0; i < packets; i++){
    //read 16 byte chunk
    Serial.readBytes((buff+i*16), 16);
    //write confirm bit
    Serial.write(1);
  }
}
void readExpect(){
  if(!expflag){
    if(Serial.available() >= sizeof(expect)){
      Serial.readBytes(expectedCommand.bytes, sizeof(expect));
    }else{
      return ;
    }
    expflag = true;
  }
}

void setup() {

  Serial.begin(19200);
  //initalize calibration profile
  calibrationLoader.loadEEPROM();

  posControl = new PositionalController(calibrationLoader.getCalibrationValues());
}
void serialEvent() {
  readExpect();
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
      trayProcessor processor(calibrationLoader.getCalibrationValues()->trays[trayIndex].x, calibrationLoader.getCalibrationValues()->trays[trayIndex].y, calibrationLoader.getCalibrationValues());
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
        expectSerialized reply = expect(CMD_CALIBRATION, false);
        Serial.write(reply.bytes, sizeof(expect));
        Serial.write(profile.bytes, sizeof(CalibrationValues));
        Serial.flush();
      } else {
        //write cmd
        CalibrationValueSerialized profile;
        memcpy(profile.bytes, buff, sizeof(CalibrationValueSerialized));
        calibrationLoader.setCalibrationValues(profile.values);
        calibrationLoader.saveToEEPROM();
        expectSerialized reply = expect(CMD_UPDATE, false);
        updateDataSerialized up = updateData(false, false, false, true);
        Serial.write(reply.bytes, sizeof(expect));
        Serial.write(profile.bytes, sizeof(CalibrationValues));
        Serial.flush();
        
      }
      
    } else if (expectedCommand.values.cmd == CMD_UPDATE) {
      //nothing to do, invalid command. Just swollow it.

    } else if (expectedCommand.values.cmd == CMD_SETPOS && !expectedCommand.values.request) {
      setPosSerialized posData;
      memcpy(posData.bytes, buff, sizeof(setPosSerialized));
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
    expflag = false;
  }
}
