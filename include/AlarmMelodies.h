#ifndef ALARMMELODIES_H
#define ALARMMELODIES_H

#include <stddef.h>
#include <stdint.h>
#include "WebServerHandlers.h"

struct MelodyNote {
  uint16_t frequency;
  uint16_t duration;
  uint16_t pauseAfter;
};

void playFanfare();
void playNokiaTune();
void playSmsTone();
void playMotorolaTone();
void playSiemensTone();
void playAlarmMelody(uint8_t sound);

#endif // ALARMMELODIES_H
