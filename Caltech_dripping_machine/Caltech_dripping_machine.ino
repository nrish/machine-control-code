#include "Positional.h"
#include "tray.h"
#include "calibration.h"
#include "events.h"

//256 byte buffer for all function calls
char buffer[256];
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
    Serial.println("got serial");
    byte payload = Serial.read();
    byte functionID = Serial.read();
    while(Serial.available() < payload);
    
    for(int i = 0; payload != 0; i++){
      buffer[i] = Serial.read();
    }
    events[functionID](buffer, &controller, &cal);
  }
}

void loop() {
   
}
