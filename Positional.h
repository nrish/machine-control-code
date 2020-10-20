#ifndef POSITIONAL_H
#define POSITIONAL_H
#include "Stepper.h"
#include "pins.h"

class PositionalController{
  private:
  Stepper *axis_x;
  Stepper *axis_y;
  int x = 0;
  int y = 0;
  float stepRatio; //mm to steps, in case its needed
  
  public:
  PositionalController(float stepRatio){
    this->axis_x = new Stepper(pins::X_DIR, pins::X_STEP);
    this->axis_y = new Stepper(pins::Y_DIR, pins::Y_STEP);;
    this->stepRatio = stepRatio;
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
  void home(){
    digitalWrite(pins::X_END, HIGH);
    axis_x->setDir(pins::X_END_DIR);
    //wait for switch to pull pin low
    for(;digitalRead(pins::X_END);){
      axis_x->step();
    }
    digitalWrite(pins::X_END, LOW);
    digitalWrite(pins::Y_END, HIGH);
     axis_y->setDir(pins::Y_END_DIR);
    //wait for switch to pull pin low
    for(;digitalRead(pins::Y_END);){
      axis_y->step();
    }
    this->x = 0;
    this->y = 0;
    digitalWrite(pins::Y_END, LOW);
  }
  void setPos(int x, int y){
    int delta_x = x - this->x;
    int delta_y = y - this->y;
    if(delta_x < 0){
      axis_x->setDir(pins::X_END_DIR);
      delta_x = -delta_x;
    }else{
      axis_x->setDir(!pins::X_END_DIR);
    }
    if(delta_y < 0){
      axis_y->setDir(pins::Y_END_DIR);
      delta_y = -delta_y;
    }else{
      axis_y->setDir(!pins::Y_END_DIR);
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
  void steps(int x, int y){
    this->x += x;
    if(x < 0){
      x = -x;
      axis_x->setDir(pins::X_END_DIR);
    }else{
      axis_x->setDir(!pins::X_END_DIR);
    }
    this->y += y;
    if(y < 0){
      y = -y;
      axis_y->setDir(pins::Y_END_DIR);
    }else{
      axis_y->setDir(!pins::Y_END_DIR);
    }
    axis_x->steps(x);
    axis_y->steps(y);
  }
  void disableSteppers(){
    digitalWrite(pins::STEPPER_ENABLE, HIGH);
  }
  void enableSteppers(){
    digitalWrite(pins::STEPPER_ENABLE, LOW);
  }
  ~PositionalController(){
    delete axis_x;
    delete axis_y;
  }
  
};
#endif
