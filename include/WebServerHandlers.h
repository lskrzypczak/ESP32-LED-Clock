/**
 * @file WebServerHandlers.h
 * @brief Shared declarations for the LED clock web UI and configuration API.
 *
 * This header exposes the global device state that the HTTP handlers need in
 * order to render status, update settings, and persist alarms. Alarm metadata
 * is intentionally declared here because both the runtime scheduler
 * (`main.cpp`) and the web API (`WebServer.cpp`) operate on the same in-memory
 * records.
 */

#ifndef WEBSERVERHANDLERS_H
#define WEBSERVERHANDLERS_H

#include <WebServer.h>
#include <WiFi.h>
#include "PCF8574.h"
#include "ESP32_RTC.h"
#include "CommonTypes.h"

/** @brief Global HTTP server instance used by the firmware web UI. */
extern WebServer server;
/** @brief Shared RTC instance used for current time reads and manual writes. */
extern ESP32RTC rtc;
/** @brief Index into `timezones[]` for the currently selected timezone. */
extern int currentTimezoneIndex;
/** @brief Current daylight-saving strategy. */
extern DstMode dstMode;
/** @brief Manual DST adjustment in seconds when `dstMode == DST_MANUAL`. */
extern int manualDstOffset;
/** @brief `true` when the device is running in fallback access-point mode. */
extern bool wifiModeAP;
/** @brief Static timezone catalog exposed to the UI. */
extern const Timezone timezones[];
/** @brief Number of valid entries in `timezones[]`. */
extern const int NUM_TIMEZONES;
/** @brief Active station SSID or the AP placeholder name. */
extern String wifiSsid;
/** @brief Active station password loaded from NVS. */
extern String wifiPassword;
/** @brief SSID advertised when the clock falls back to access-point mode. */
extern const char* AP_SSID;

/** @brief Maximum number of alarm records the firmware stores and evaluates. */
const int MAX_ALARMS = 6;

/**
 * @brief Selectable alarm sound identifiers.
 *
 * The numeric values are persisted to NVS and exchanged with the web UI, so
 * they must remain stable across firmware revisions.
 */
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

/**
 * @brief Supported recurrence modes for alarms.
 *
 * Like `AlarmSound`, these values are serialized into the compact alarm
 * storage string and therefore form part of the persistence format.
 */
enum AlarmSchedule : uint8_t {
  ALARM_SCHEDULE_DAILY = 0,
  ALARM_SCHEDULE_WEEKDAYS = 1,
  ALARM_SCHEDULE_WEEKENDS = 2,
  ALARM_SCHEDULE_COUNT
};

/**
 * @brief In-memory representation of one alarm entry.
 *
 * Alarm records are persisted as a compact pipe-separated list in NVS under
 * the `alarm/alarms` key. Each record stores the fields in this order:
 * `enabled,hour,minute,sound,schedule,title`.
 *
 * The `title` field is percent-encoded before storage so commas and pipe
 * characters cannot corrupt parsing.
 */
struct AlarmConfig {
  /** @brief Whether the scheduler should trigger this alarm. */
  bool enabled;
  /** @brief Trigger hour in 24-hour format. */
  uint8_t hour;
  /** @brief Trigger minute. */
  uint8_t minute;
  /** @brief Melody or tone identifier from `AlarmSound`. */
  uint8_t sound;
  /** @brief Recurrence mode from `AlarmSchedule`. */
  uint8_t schedule;
  /** @brief User-visible label shown in the web UI alarm tabs. */
  String title;
};

/** @brief Fixed-size alarm storage used by both the scheduler and web API. */
extern AlarmConfig alarms[MAX_ALARMS];
/** @brief Number of valid entries currently stored in `alarms`. */
extern int alarmCount;

/** @brief Returns the effective GMT offset in seconds for RTC/NTP sync. */
extern int getEffectiveGmtOffset();
/** @brief Returns the effective DST adjustment in seconds. */
extern int getEffectiveDstOffset();
/** @brief Plays a simple multi-beep notification. */
extern void beep(int count);
/** @brief Plays the alarm sound associated with an `AlarmSound` value. */
extern void playAlarmSound(uint8_t sound);

/** @brief Loads WiFi credentials from NVS into the global variables. */
extern void loadWifiCredentials();
/** @brief Saves WiFi credentials to NVS and updates the globals. */
extern void saveWifiCredentials(const String &ssid, const String &password);

/**
 * @brief Loads alarm definitions from NVS into `alarms`.
 *
 * The loader expects the current six-field record layout exactly and ignores
 * malformed or out-of-range records.
 */
extern void loadAlarms();
/**
 * @brief Persists the current `alarms` array to NVS.
 *
 * Records are written as a single compact string to minimize storage overhead
 * and to keep the format easy to inspect when debugging.
 */
extern void saveAlarms();

/** @brief Serves the main single-page web application. */
void handleRoot();
/** @brief Responds to `/favicon.ico` without sending a body. */
void handleFavicon();
/** @brief Returns the current device status and alarm list as JSON. */
void handleApiStatus();
/** @brief Accepts manual time changes from the web UI. */
void handleApiSetTime();
/** @brief Saves WiFi credentials received from the web UI. */
void handleApiWifi();
/** @brief Validates and stores the full alarm list submitted by the web UI. */
void handleApiAlarms();
/** @brief Plays a requested alarm sound for preview/testing. */
void handleApiAlarmTest();
/** @brief Updates timezone and DST settings from the web UI. */
void handleApiTimezone();
/** @brief Handles unknown routes, delegating non-API paths to the SPA. */
void handleNotFound();

/** @brief Registers all HTTP routes used by the device web server. */
void setupWebServer();

#endif // WEBSERVERHANDLERS_H
