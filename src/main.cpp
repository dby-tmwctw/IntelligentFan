#include <Arduino.h>
#include <SoftwareSerial.h>
#include <fan_control.h>
#include <dht11.h>
#include <audio_player.h>
#include <block_detector.h>

const int fan_pin = 3;
const int sensor_pin = 4;
const int rx_pin = 5;
const int tx_pin = 6;
const int ultrasonic_trig_pin = 7;
const int ultrasonic_echo_pin = 8;
const int test_pin1 = 10;
const int test_pin2 = 11;
int timer1_counter;
volatile int counter = 0;
volatile long overflow_number = 10;
volatile bool awaken = true;
SoftwareSerial voice_control(5, 6);
SoftwareSerial BT(9, 12);
fan_control fan_control(fan_pin);
dht11 environment_data;
audio_player audio_player(rx_pin, tx_pin);
block_detector block_detector(ultrasonic_trig_pin, ultrasonic_echo_pin);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  voice_control.begin(9600);
  BT.begin(9600);
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

void block_sensor(int previous_value)
{
  bool blocked1 = block_detector.blocked();
  delay(10);
  bool blocked2 = block_detector.blocked();
  if (blocked1 || blocked2)
  {
    audio_player.play_blocked_warning(voice_control);
    fan_control.fan_switch(false);
    while (true)
    {
      bool try_blocked = block_detector.blocked();
      if (try_blocked == false)
      {
        fan_control.write_value = previous_value;
        fan_control.run();
        break;
      }
      delay(100);
    }
  }
}

void temperature_reaction()
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
  }
  overflow_number = 1000;
}

byte read_bluetooth()
{
  BT.listen();
  byte true_message = 0;
  char first_char = BT.read();
  if (first_char != -1)
  {
    delay(10);
    char second_char = BT.read();
    if (second_char != -1)
    {
      true_message = 10 * ((byte)first_char - 48) + ((byte)second_char - 48);
    } else
    {
      true_message = (byte)first_char - 48;
    }
  }

  return true_message;
}

void loop() {
  // put your main code here, to run repeatedly:
  counter += 1;
  // delay(10);
  int received = 0;
  received = Serial.parseInt();
  if (received == 0)
  {
    received = read_bluetooth();
    if (received != 0)
    {
      awaken = true;
    }
  }
  if (received == 100)
  {
    audio_player.play_response(voice_control);
    awaken = true;
  }
  if (counter > overflow_number && awaken == false)
  {
    counter = 0;
    if (fan_control.write_value != 255)
    {
      int previous_value = fan_control.write_value;
      block_sensor(previous_value);
    }
    environment_data.read(sensor_pin);
    int temperature = environment_data.temperature;
    if (temperature >= 30 && fan_control.write_value == 255)
    {
      temperature_reaction();
    }
  }
  if (awaken)
  {
    overflow_number = 10;
    switch (received)
    {
      case 1:
      {
        fan_control.fan_switch(true);
        awaken = false;
        break;
      }
      case 2:
      {
        fan_control.fan_switch(false);
        awaken = false;
        break;
      }
      case 3:
      {
        fan_control.fan_mode(1);
        awaken = false;
        break;
      }
      case 4:
      {
        fan_control.fan_mode(2);
        awaken = false;
        break;
      }
      case 5:
      {
        fan_control.fan_mode(3);
        awaken = false;
        break;
      }
      case 6:
      {
        fan_control.fan_change(true);
        awaken = false;
        break;
      }
      case 7:
      {
        fan_control.fan_change(false);
        awaken = false;
        break;
      }
      case 8:
      {
        environment_data.read(sensor_pin);
        int temperature = environment_data.temperature;
        audio_player.play_temperature(voice_control, temperature);
        awaken = false;
        break;
      }
      case 9:
      {
        environment_data.read(sensor_pin);
        int humidity = environment_data.humidity;
        audio_player.play_humidity(voice_control, humidity);
        awaken = false;
        break;
      }
    }
  }
}
