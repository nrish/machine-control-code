#ifndef POSITIONAL_H
#define POSITIONAL_H
#include "Stepper.h"
#include "calibration.h"

class PositionalController{
  const static uint16_t FAST_MODE = 250;
  const static uint16_t SLOW_MODE = 500;
  private:
  Stepper *axis_x;
  Stepper *axis_y;
  int x = 0;
  int y = 0;
  CalibrationValues* values;
  public:
  PositionalController(CalibrationValues* values){
    this->axis_x = new Stepper(pins::X_DIR, pins::X_STEP);
    this->axis_y = new Stepper(pins::Y_DIR, pins::Y_STEP);
    this->values = values;
    //setup homing
    pinMode(pins::X_END, INPUT);
    pinMode(pins::Y_END, INPUT);
    
    //make sure servo doesn't drag tube
    pinMode(pins::PWM_SERVO, OUTPUT);
    analogWrite(pins::PWM_SERVO, 160);

    //enable steppers
    pinMode(pins::STEPPER_ENABLE, OUTPUT);
    digitalWrite(pins::STEPPER_ENABLE, LOW);
  }
  //moves head till it triggers limit switches
  void home(){
    digitalWrite(pins::X_END, HIGH);
    axis_x->setDir(values->X_END_DIR);
    //wait for switch to pull pin low
    for(;digitalRead(pins::X_END);){
      axis_x->step();
    }
    digitalWrite(pins::X_END, LOW);
    digitalWrite(pins::Y_END, HIGH);
     axis_y->setDir(values->Y_END_DIR);
    //wait for switch to pull pin low
    for(;digitalRead(pins::Y_END);){
      axis_y->step();
    }
    this->x = 0;
    this->y = 0;
    digitalWrite(pins::Y_END, LOW);
  }
  //uses stored position of head to move head to new location, might not always be correct!
  void setPos(int x, int y){
    int delta_x = x - this->x;
    int delta_y = y - this->y;
    if(delta_x < 0){
      axis_x->setDir(values->X_END_DIR);
      delta_x = -delta_x;
    }else{
      axis_x->setDir(!values->X_END_DIR);
    }
    if(delta_y < 0){
      axis_y->setDir(values->Y_END_DIR);
      delta_y = -delta_y;
    }else{
      axis_y->setDir(!values->Y_END_DIR);
    }
    axis_x->steps(delta_x);
    axis_y->steps(delta_y);
    this->x = x;
    this->y = y;
  }
  void setXPos(int x){
    setPos(x,this->y);
  }
  void setYPos(int y){
    setPos(this->x,y);
  }
  /*
   * moves head by delta(x,y) steps
   * x - steps in x direction
   * y - steps in y direction
   * negative steps step backwards
   */
  void steps(int x, int y){
    this->x += x;
    if(x < 0){
      x = -x;
      axis_x->setDir(values->X_END_DIR);
    }else{
      axis_x->setDir(!values->X_END_DIR);
    }
    this->y += y;
    if(y < 0){
      y = -y;
      axis_y->setDir(values->Y_END_DIR);
    }else{
      axis_y->setDir(!values->Y_END_DIR);
    }
    axis_x->steps(x);
    axis_y->steps(y);
  }
  //disables stepper drivers
  void disableSteppers(){
    digitalWrite(pins::STEPPER_ENABLE, HIGH);
  }
  //renable stepper drivers
  void enableSteppers(){
    digitalWrite(pins::STEPPER_ENABLE, LOW);
  }
  void setFastMode(bool mode){
    if(mode){
      axis_x->setUsecs(FAST_MODE);
      axis_y->setUsecs(FAST_MODE);
    }else{
      axis_x->setUsecs(SLOW_MODE);
      axis_y->setUsecs(SLOW_MODE);
    }
      
  }
  ~PositionalController(){
    delete axis_x;
    delete axis_y;
  }
  
};
#endif
