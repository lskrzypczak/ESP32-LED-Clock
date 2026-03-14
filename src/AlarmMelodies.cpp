#include <Arduino.h>
#include "AlarmMelodies.h"

extern void playNote(unsigned int frequency, unsigned long duration);

static void playMelody(const MelodyNote *notes, size_t noteCount) {
  for (size_t i = 0; i < noteCount; i++) {
    playNote(notes[i].frequency, notes[i].duration);
    if (notes[i].pauseAfter > 0) {
      delay(notes[i].pauseAfter);
    }
  }
}

void playFanfare() {
  static const MelodyNote melody[] = {
    {523, 200, 50},
    {659, 200, 50},
    {784, 200, 50},
    {1047, 400, 100}
  };
  playMelody(melody, sizeof(melody) / sizeof(melody[0]));
}

void playNokiaTune() {
  static const MelodyNote melody[] = {
    {1319, 140, 20}, {1175, 140, 20}, {740, 280, 30}, {831, 280, 30},
    {1109, 140, 20}, {988, 140, 20}, {587, 280, 30}, {659, 280, 80},
    {988, 140, 20}, {880, 140, 20}, {554, 280, 30}, {659, 280, 30},
    {880, 140, 20}, {784, 140, 20}, {494, 280, 30}, {554, 320, 120}
  };
  playMelody(melody, sizeof(melody) / sizeof(melody[0]));
}

void playSmsTone() {
  static const MelodyNote melody[] = {
    {1568, 90, 30}, {1760, 90, 30}, {2093, 150, 90},
    {1760, 90, 30}, {1568, 120, 30}, {1319, 220, 80}
  };
  playMelody(melody, sizeof(melody) / sizeof(melody[0]));
}

void playMotorolaTone() {
  static const MelodyNote melody[] = {
    {988, 120, 30}, {1319, 120, 30}, {1568, 200, 50},
    {1319, 120, 30}, {988, 120, 30}, {784, 240, 80},
    {988, 120, 30}, {1319, 120, 30}, {1760, 260, 100}
  };
  playMelody(melody, sizeof(melody) / sizeof(melody[0]));
}

void playSiemensTone() {
  static const MelodyNote melody[] = {
    {1175, 110, 25}, {1319, 110, 25}, {1568, 110, 25}, {1760, 180, 40},
    {1568, 110, 25}, {1319, 110, 25}, {1175, 180, 40}, {988, 260, 80}
  };
  playMelody(melody, sizeof(melody) / sizeof(melody[0]));
}

void playAlarmMelody(uint8_t sound) {
  switch (sound) {
    case ALARM_SOUND_FANFARE:
      playFanfare();
      break;
    case ALARM_SOUND_NOKIA:
      playNokiaTune();
      break;
    case ALARM_SOUND_SMS:
      playSmsTone();
      break;
    case ALARM_SOUND_MOTOROLA:
      playMotorolaTone();
      break;
    case ALARM_SOUND_SIEMENS:
      playSiemensTone();
      break;
    default:
      break;
  }
}
