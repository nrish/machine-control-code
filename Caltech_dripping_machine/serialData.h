#ifndef SERIAL_DATA_H
#define SERIAL_DATA_H
/*
 * this file contains all the structures and unions required to easily serialize data and send it accross the serial port or write to EEPROM.
 */

struct __attribute__((__packed__)) Tray{
  int x; // index 0
  int y; // index 1
};

struct __attribute__((__packed__)) StartData{
  byte start_mode; //start mode can be 0 or 1 respectively normal run and calibration
  uint32_t mills; //ms/w
  byte end_well_index;
  byte tray_index;
  Tray trayData;
};

union StartDataSerialized{
  StartData StartData;
  byte bytes[sizeof(StartData)];
};


//EEPROM serializable data

struct __attribute__((__packed__)) CalibrationValues{
  byte WELL_DIST_X;
  byte WELL_DIST_Y;
  
  byte TRAY_DIST_X;
  byte TRAY_DIST_Y;

  bool Y_END_DIR;
  bool X_END_DIR;

  Tray trays[8];
};
union EEPROMData{
  byte bytes[sizeof(CalibrationValues)];
  CalibrationValues values;
};


#endif
