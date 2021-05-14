#ifndef PIN_H
#define PIN_H
//these can move to #define, but to avoid reuse they should stay in the namespace.
namespace pins{
  const int X_STEP = 2;
  const int Y_STEP = 3;
  const int X_DIR = 5;
  const int Y_DIR = 6;
  const int Y_END = 10; //pulled low when on
  const int X_END = 9;  //pulled low when on
  const int Y_END_DIR = true;
  const int X_END_DIR = true;
  const int PWM_SERVO = 11;
  const int STEPPER_ENABLE = 8; //pull low
};
#endif
