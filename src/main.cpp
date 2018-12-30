#include <Arduino.h>
#include <SoftwareSerial.h>
#include <fan_control.h>
#include <dht11.h>
#include <audio_player.h>

const int fan_pin = 3;
const int sensor_pin = 4;
const int rx_pin = 5;
const int tx_pin = 6;
const int test_pin1 = 10;
const int test_pin2 = 11;
int timer1_counter;
volatile int counter = 0;
volatile long overflow_number = 10;
SoftwareSerial voice_control(5, 6);
fan_control fan_control(fan_pin);
dht11 environment_data;
audio_player audio_player(rx_pin, tx_pin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  voice_control.begin(9600);
  pinMode(test_pin1, OUTPUT);
  pinMode(test_pin2, OUTPUT);
  analogWrite(fan_pin, 255);
  audio_player.set_volume(voice_control, 30);
  audio_player.play_welcome(voice_control);
  // noInterrupts();
  // TCCR1A = 0;
  // TCCR1B = 0;
  // timer1_counter = 3036;
  // TCNT1 = timer1_counter;
  // TCCR1B |= (1 << CS10);
  // TCCR1B |= (1 << CS12);
  // TIMSK1 |= (1 << TOIE1);
  // interrupts();
}

// ISR(TIMER1_OVF_vect)
// {
//   TCNT1 = timer1_counter;
//   environment_data.read(sensor_pin);
//   int temperature = environment_data.temperature;
//   if (temperature >= 25)
//   {
//     audio_player.warning(voice_control);
//   }
// }

void loop() {
  // put your main code here, to run repeatedly:
  counter += 1;
  // delay(10);
  int received = 0;
  received = Serial.parseInt();
  if (counter > overflow_number)
  {
    counter = 0;
    environment_data.read(sensor_pin);
    int temperature = environment_data.temperature;
    if (temperature >= 30)
    {
      audio_player.warning(voice_control);
      int decision = 0;
      while (true)
      {
        decision = Serial.parseInt();
        if (decision != 0)
        {
          break;
        }
      }
      if (decision == 11)
      {
        fan_control.fan_switch(true);
      } else if (decision != 12)
      {
        received = decision;
      }
      overflow_number = 1000;
    }
  }
  switch (received)
  {
    case 1:
    {
      fan_control.fan_switch(true);
      break;
    }
    case 2:
    {
      fan_control.fan_switch(false);
      break;
    }
    case 3:
    {
      fan_control.fan_mode(1);
      break;
    }
    case 4:
    {
      fan_control.fan_mode(2);
      break;
    }
    case 5:
    {
      fan_control.fan_mode(3);
      break;
    }
    case 6:
    {
      fan_control.fan_change(true);
      break;
    }
    case 7:
    {
      fan_control.fan_change(false);
      break;
    }
    case 8:
    {
      environment_data.read(sensor_pin);
      int temperature = environment_data.temperature;
      audio_player.play_temperature(voice_control, temperature);
      break;
    }
    case 9:
    {
      environment_data.read(sensor_pin);
      int humidity = environment_data.humidity;
      audio_player.play_humidity(voice_control, humidity);
      break;
    }
  }
}
