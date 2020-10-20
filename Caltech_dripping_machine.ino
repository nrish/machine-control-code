#include "Positional.h"
#include "tray.h"
PositionalController* posControl;
tray trays[] = {tray(190, 1150), tray(7427,1150), tray(190, 4700)};
boolean manual = false;
/**
 * By Riley Ramirez and Forrest Ramirez Oct 16 2020
 * For Caltech
 */
void setup() {
  //parameter is the mm to step conversion ratio.
  posControl = new PositionalController(1.00);
  posControl->home();
  Serial.begin(19200);
  byte buf[10];
  while(!Serial.available());
  Serial.readBytes(buf, 10);
  unsigned long millsPerWell = ((uint32_t*)(buf))[0];
  uint16_t startWell = ((uint16_t*)(buf))[2];
  uint16_t endWell = ((uint16_t*)(buf))[3];
  uint16_t trayIndex = ((uint16_t*)(buf))[4];
  Serial.write(1);
  trays[trayIndex].setEndIndex(endWell);
  trays[trayIndex].start(posControl);
  trays[trayIndex].process(posControl, millsPerWell);
  
}
void loop() {
  if(!manual)
    asm volatile ("jmp 0");
  Serial.println("enter X and Y (enter q to quit):");
  while(!Serial.available());
  if(Serial.peek() == "q"){
    while(Serial.available()) Serial.read();
    manual = false;
    return;
  }
  int x = Serial.parseInt();
  Serial.readString();
  while(!Serial.available());
  int y = Serial.parseInt();
  
  Serial.readString();
  posControl->setPos(x,y);
}
