#include "fan_control.h"
#include <Arduino.h>

fan_control::fan_control(int pin)
{
  low = 180;
  middle = 120;
  high = 0;
  write_value = 255;
  fan_pin = pin;
  pinMode(fan_pin, OUTPUT);
}

void fan_control::run()
{
  analogWrite(fan_pin, write_value);
}

void fan_control::fan_switch(bool whether_on)
{
  if (whether_on)
  {
    write_value = middle;
  } else
  {
    write_value = 255;
  }
  fan_control::run();
}

void fan_control::fan_mode(int mode)
{
  switch(mode)
  {
    case 1:
    {
      write_value = low;
      break;
    }
    case 2:
    {
      write_value = middle;
      break;
    }
    case 3:
    {
      write_value = high;
      break;
    }
   }
   fan_control::run();
}

void fan_control::fan_change(bool increase)
{
  if (increase)
  {
    write_value -= 20;
  } else
  {
    write_value += 20;
  }
  fan_control::run();
}
