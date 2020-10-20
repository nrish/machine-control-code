#ifndef stepper_h
#define stepper_h
class Stepper{
  private:
    int dirPin = 0;
    int stepPin = 0;
    //clockwise true, counterclockwise false
    bool dir = true;
  public:
    Stepper(int dir, int step){
      this->dirPin = dir;
      this->stepPin = step; 
      pinMode(dirPin, OUTPUT);
      pinMode(stepPin, OUTPUT);
       
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
      delay(1);
      digitalWrite(stepPin, LOW);
      delay(1);
    }
    void steps(int steps){
      for(int i = 0; i < steps; i++){
        step();
      }
    }
};
#endif
