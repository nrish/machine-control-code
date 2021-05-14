#include "Positional.h"
#include "tray.h"
//term code, trigger to halt well loop and reset arduino
#define TERM_CODE 0x55
PositionalController* posControl;
tray trays[] = {tray(190, 1150), tray(7427,1150), tray(190, 4700)};
bool manual = false;
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
  //wait for serial to send bytes
  while(!Serial.available());
  Serial.readBytes(buf, 32);
  //first 4 bytes are long specifiying t/well
  unsigned long millsPerWell = ((uint32_t*)(buf))[0];
  uint8_t endWell = ((uint8_t*)(buf))[4];
  //trayindex sepecifies the index of the tray. Really needs to be [0-8]
  uint8_t trayIndex = ((uint8_t*)(buf))[4];
  Serial.write(1);
  trays[trayIndex].setEndIndex(endWell);
  trays[trayIndex].start(posControl);
  trays[trayIndex].process(posControl, millsPerWell);
  //reset after processing is done.
  
  asm volatile ("jmp 0");
}
void loop() {
  //reset, as shouldn't be here
  asm volatile ("jmp 0");
}
