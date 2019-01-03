#include <Arduino.h>
#include <SoftwareSerial.h>
#include <fan_control.h>
#include <dht11.h>
#include <audio_player.h>
#include <block_detector.h>

const int fan_pin = 3; // This pin connects to fan itself
const int sensor_pin = 4; // This connects to temperature and humidity sensor
const int rx_pin = 5; // This pin send information to JQ6500 voice controller
const int tx_pin = 6; // This pin receives information from JQ6500 voice controller
const int ultrasonic_trig_pin = 7; // This pin send trigger signal to ultrasonic sensor
const int ultrasonic_echo_pin = 8; // This pin receives signal from ultrasonic sensor
const int test_pin1 = 10; // This pin flashes green LED
const int test_pin2 = 11; // This pin flashes yellow LED
volatile int counter = 0; // This integer act as a counter
volatile long overflow_number = 10; // When the previous integer becomes larger than this, read from temperature sensor
volatile bool awaken = true; // Marker to mark whether the intelligent fan is in awaken mode
SoftwareSerial voice_control(5, 6); // This communicates with JQ6500
SoftwareSerial BT(9, 12); // This communicates with the bluetooth module
fan_control fan_control(fan_pin); // This controls fan
dht11 environment_data; // This contorls sensor
audio_player audio_player(rx_pin, tx_pin); // This communicates with JQ6500
block_detector block_detector(ultrasonic_trig_pin, ultrasonic_echo_pin); // This communicates with ultrasonic sensor


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  voice_control.begin(9600);
  BT.begin(9600);
  pinMode(test_pin1, OUTPUT);
  pinMode(test_pin2, OUTPUT);
  analogWrite(fan_pin, 255);
  // When switched on, set default volume to 30 and play welcome audio
  audio_player.set_volume(voice_control, 30);
  audio_player.play_welcome(voice_control);
}

void block_sensor(int previous_value)
{
  // This function checkes whether the fan is blocked. If so, stop the fan till
  // the blockage is removed
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
  /*
  This function plays warning and ask user to make decision whether to turn the
  fan on when it is called
  */
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
  /*
  This function reads information from HC05 bluetooth module. The information
  send to the module must be an integer between 0 and 99
  */
  delay(10);
  BT.listen();
  volatile byte true_message = 0;
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
  // Getting information from voice input
  int received = 0;
  received = Serial.parseInt();
  if (received == 0)
  {
    // If nothing is received from voice input, try bluetooth
    received = read_bluetooth();
    if (received != 0)
    {
      awaken = true;
    }
  }
  if (received == 50)
  {
    // When its name is being asked, answer accodingly
    audio_player.play_name(voice_control);
  }
  if (received == 100)
  {
    // This awakenes the intelligent fan
    audio_player.play_response(voice_control);
    awaken = true;
  }
  if (counter > overflow_number && awaken == false)
  {
    // When counter overflows, check termperature and whether it is blocked
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
      case 13:
      {
        digitalWrite(test_pin1, HIGH);
        awaken = false;
        break;
      }
      case 14:
      {
        digitalWrite(test_pin2, HIGH);
        awaken = false;
        break;
      }
      case 15:
      {
        digitalWrite(test_pin1, LOW);
        awaken = false;
        break;
      }
      case 16:
      {
        digitalWrite(test_pin2, LOW);
        awaken = false;
        break;
      }
      case 17:
      {
        audio_player.play_song(voice_control);
        awaken = false;
        break;
      }
      case 18:
      {
        audio_player.play_another(voice_control);
        awaken = false;
        break;
      }
    }
  }
}
