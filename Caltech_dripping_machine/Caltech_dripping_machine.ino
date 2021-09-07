#include "Positional.h"
#include "tray.h"
#include "calibration.h"
#include "events.h"

//256 byte buffer for all function calls
char buffer[64];
byte size;
calibrator cal;
PositionalController controller(cal.getValues());
void setup() {
  Serial.begin(9600);
}

void serialEvent() {
  //format for receving data
  // [ 1 byte, payload bytes ] [ 1 byte, function id ] [ remaining bytes ]
  if(Serial.available() >= 2){  
    byte payload = Serial.read();
    //since no payloads are greater than 64 bytes we hard limit
    if(payload > 63)
      payload = 63;
    byte functionID = Serial.read();
    while(Serial.available() < payload);
    
    for(int i = 0; i < payload; i++){
      buffer[i] = Serial.read();
    }
    if(functionID >= 0 && functionID <= 10)
      events[functionID](buffer, &controller, &cal);
      Serial.write(0);
      Serial.write(0xFF);
  }
}

void loop() {
   
}
