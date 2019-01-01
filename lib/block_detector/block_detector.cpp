#include "block_detector.h"
#include <Arduino.h>

block_detector::block_detector(int trig_pin, int echo_pin)
{
  TRIG = trig_pin;
  ECHO = echo_pin;
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

float block_detector::distance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  float distance = pulseIn(ECHO, HIGH) / 58.30;
  return distance;
}

bool block_detector::blocked()
{
  float distance = block_detector::distance();
  if (distance <= 11 || (distance >= 2220 && distance <= 2225))
  {
    return true;
  }

  return false;
}
