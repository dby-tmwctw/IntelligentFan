#include <Arduino.h>

#ifndef block_detector_h
#define block_detector_h

class block_detector
{
private:
  int TRIG;
  int ECHO;
  float distance();
public:
  block_detector(int trig_pin, int echo_pin);
  bool blocked();
};

#endif
