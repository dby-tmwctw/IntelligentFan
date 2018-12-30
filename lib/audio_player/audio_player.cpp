#include "audio_player.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

audio_player::audio_player(int rx_pin, int tx_pin)
{
  RX_pin = rx_pin;
  TX_pin = tx_pin;
}

void audio_player::play_audio(SoftwareSerial voice_control, byte audio_number, bool must_complete)
{
  // Play the song as specified by audio_number. If must_complete is true, the
  // next song will play only after this audio is completed
  byte play_command[] = {0x7E, 0x04, 0x03, 0x00, 0x00, 0xEF};
  play_command[4] = 0x00 + audio_number;
  voice_control.write(play_command, 6);
  if (!(must_complete))
  {
    delay(1000);
  }
}

void audio_player::play_number(SoftwareSerial voice_control, int number)
{
  if (number <= 10)
  {
    audio_player::play_audio(voice_control, number, true);
  } else
  {
    int tenth_place = (int) (number / 10);
    int number_place = number - (10 * tenth_place);
    audio_player::play_audio(voice_control, tenth_place, true);
    audio_player::play_audio(voice_control, 10, true);
    if (number_place != 0)
    {
      audio_player::play_audio(voice_control, number_place, true);
    }
  }
}

void audio_player::play_welcome(SoftwareSerial voice_control)
{
  audio_player::play_audio(voice_control, 15, true);
}

void audio_player::play_temperature(SoftwareSerial voice_control, int temperature)
{
  audio_player::play_audio(voice_control, 13, true);
  audio_player::play_number(voice_control, temperature);
  audio_player::play_audio(voice_control, 11, true);
}

void audio_player::play_humidity(SoftwareSerial voice_control, int humidity)
{
  audio_player::play_audio(voice_control, 12, true);
  audio_player::play_number(voice_control, humidity);
}

void audio_player::warning(SoftwareSerial voice_control)
{
  audio_player::play_audio(voice_control, 14, true);
}

void audio_player::set_volume(SoftwareSerial voice_control, int volume)
{
  // Set the volume to be a specified number between 0 - 30
  byte adjust_volume[] = {0x7E, 0x03, 0x06, 0x00, 0xEF};
  adjust_volume[3] = 0x00 + volume;
  if (volume >= 0 && volume <= 30)
  {
    voice_control.write(adjust_volume, 5);
  }
}
