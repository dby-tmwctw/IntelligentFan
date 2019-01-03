/*
This class contains functions playing audios from JQ6500 module
*/

#include <Arduino.h>
#include <SoftwareSerial.h>

#ifndef audio_player_h
#define audio_player_h

class audio_player
{
private:
  int RX_pin;
  int TX_pin;
  int now_song;
  void play_audio(SoftwareSerial voice_control, byte audio_number, bool must_complete);
  void play_number(SoftwareSerial voice_control, int number);
public:
  audio_player(int rx_pin, int tx_pin);
  void play_welcome(SoftwareSerial voice_control);
  void play_response(SoftwareSerial voice_control);
  void play_blocked_warning(SoftwareSerial voice_control);
  void play_name(SoftwareSerial voice_control);
  void play_temperature(SoftwareSerial voice_control, int temperature);
  void play_humidity(SoftwareSerial voice_control, int humidity);
  void warning(SoftwareSerial voice_control);
  void play_song(SoftwareSerial voice_control);
  void play_another(SoftwareSerial voice_control);
  void set_volume(SoftwareSerial voice_control, int volume);
};

#endif
