#ifndef stepper_h
#define stepper_h
//dont store position here, this object is just a driver for a stepper.
class Stepper{
  private:
    //specifies physical pin to change the state of for direction
    int dirPin = 0;
    //specifies physical pin to change the state of for stepping driver
    int stepPin = 0;
    //clockwise true, counterclockwise false
    bool dir = true;
    //time between steps (in usec)
    uint16_t stepUsecs;
  public:
    Stepper(int dir, int step){
      this->dirPin = dir;
      this->stepPin = step; 
      this->stepUsecs = 500;
      pinMode(dirPin, OUTPUT);
      pinMode(stepPin, OUTPUT);
      //not sure, but the port A/B register is reset on jmp 0, so best to make sure
      digitalWrite(dirPin, LOW);
      digitalWrite(stepPin, LOW);
    }
    void setDir(boolean b){
      this->dir = b;
      digitalWrite(dirPin, dir);
    }
    boolean getDir(){
      return dir;
    }
    void flipDir(){
      dir = !dir;
      digitalWrite(dirPin, dir);
    }
    void step(){
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepUsecs);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepUsecs);
      
    }
    void steps(int steps){
      for(int i = 0; i < steps; i++){
        step();
      }
    }
    void setUsecs(uint16_t usec){
      this->stepUsecs = usec;
    }
    
};
#endif
