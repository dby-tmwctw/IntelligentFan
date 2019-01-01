#include <Arduino.h>

#ifndef fan_control_h
#define fan_control_h

class fan_control
{
private:
  int fan_pin;
  int low;
  int middle;
  int high;
public:
  volatile int write_value;
  fan_control(int pin); // Initialiser
  void fan_switch(bool whether_on); // Switch on or off the fan
  void fan_mode(int mode); // Change fan to a certain mode
  void fan_change(bool increase); // Change fan speed
  void run();
};

#endif
