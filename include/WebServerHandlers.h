#ifndef WEBSERVERHANDLERS_H
#define WEBSERVERHANDLERS_H

#include <WebServer.h>
#include <WiFi.h>
#include "PCF8574.h"
#include "ESP32_RTC.h"
#include "CommonTypes.h"

// External declarations for global variables needed by web handlers
extern WebServer server;
extern ESP32RTC rtc;
extern int currentTimezoneIndex;
extern DstMode dstMode;
extern int manualDstOffset;
extern bool wifiModeAP;
extern const Timezone timezones[];
extern const int NUM_TIMEZONES;
extern String wifiSsid;
extern String wifiPassword;
extern const char* AP_SSID;

// Alarm state
const int MAX_ALARMS = 6;

enum AlarmSound : uint8_t {
  ALARM_SOUND_SIREN = 0,
  ALARM_SOUND_BEEP = 1,
  ALARM_SOUND_FANFARE = 2,
  ALARM_SOUND_NOKIA = 3,
  ALARM_SOUND_SMS = 4,
  ALARM_SOUND_MOTOROLA = 5,
  ALARM_SOUND_SIEMENS = 6,
  ALARM_SOUND_COUNT
};

enum AlarmSchedule : uint8_t {
  ALARM_SCHEDULE_DAILY = 0,
  ALARM_SCHEDULE_WEEKDAYS = 1,
  ALARM_SCHEDULE_WEEKENDS = 2,
  ALARM_SCHEDULE_COUNT
};

struct AlarmConfig {
  bool enabled;
  uint8_t hour;
  uint8_t minute;
  uint8_t sound;
  uint8_t schedule;
};

extern AlarmConfig alarms[MAX_ALARMS];
extern int alarmCount;

// Shared helper functions (implemented in main.cpp)
extern int getEffectiveGmtOffset();
extern int getEffectiveDstOffset();
extern void beep(int count);
extern void playAlarmSound(uint8_t sound);

// WiFi credential persistence
extern void loadWifiCredentials();
extern void saveWifiCredentials(const String &ssid, const String &password);

// Alarm persistence
extern void loadAlarms();
extern void saveAlarms();

// Web server handler functions
void handleRoot();
void handleFavicon();
void handleApiStatus();
void handleApiSetTime();
void handleApiWifi();
void handleApiAlarms();
void handleApiAlarmTest();
void handleApiTimezone();
void handleNotFound();

// Web server setup function
void setupWebServer();

#endif // WEBSERVERHANDLERS_H
