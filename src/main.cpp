#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include "PCF8574.h"
#include "ESP32_RTC.h"
#include "Digits5x8.h"
#include "CommonTypes.h"
#include "WebServerHandlers.h"

// Default WiFi configuration
const char* DEFAULT_WIFI_SSID = "Your_AP";        // Replace with your WiFi SSID
const char* DEFAULT_WIFI_PASSWORD = "PASSWORD"; // Replace with your WiFi password

// Stored/persistent WiFi configuration (loaded from NVS)
String wifiSsid;
String wifiPassword;
Preferences prefs;

// Access Point configuration (fallback mode)
const char* AP_SSID = "LEDClock";
const char* AP_PASSWORD = "clock1234";  // Minimum 8 characters for WPA2

// Web server on port 80
WebServer server(80);

// Timezone configuration

const Timezone timezones[] = {
  {"UTC", "Coordinated Universal Time", 0, 0},
  {"CET", "Central European Time (UTC+1)", 3600, 3600},
  {"CEST", "Central European Summer Time (UTC+2)", 7200, 0},
  {"EST", "Eastern Standard Time (UTC-5)", -18000, 3600},
  {"EDT", "Eastern Daylight Time (UTC-4)", -14400, 0},
  {"PST", "Pacific Standard Time (UTC-8)", -28800, 3600},
  {"PDT", "Pacific Daylight Time (UTC-7)", -25200, 0},
  {"GMT", "Greenwich Mean Time", 0, 3600},
  {"BST", "British Summer Time (UTC+1)", 3600, 0},
  {"JST", "Japan Standard Time (UTC+9)", 32400, 0}
};

const int NUM_TIMEZONES = sizeof(timezones) / sizeof(timezones[0]);
int currentTimezoneIndex = 1;  // Default to CET (index 1)

// DST (Daylight Saving Time) configuration
DstMode dstMode = DST_AUTO;  // Default to automatic DST
int manualDstOffset = 0;     // Manual DST offset in seconds (0 = no DST, 3600 = +1 hour)

// Get effective DST offset based on current mode
int getEffectiveDstOffset() {
  if (dstMode == DST_AUTO) {
    return timezones[currentTimezoneIndex].dstOffset;
  } else {
    return manualDstOffset;
  }
}

// Get effective GMT offset (timezone base + manual adjustments if in manual mode)
int getEffectiveGmtOffset() {
  if (dstMode == DST_AUTO) {
    return timezones[currentTimezoneIndex].gmtOffset;
  } else {
    // In manual mode, use timezone base GMT + manual DST
    return timezones[currentTimezoneIndex].gmtOffset + manualDstOffset;
  }
}

// WiFi credential persistence
void loadWifiCredentials() {
  prefs.begin("wifi", true);
  wifiSsid = prefs.getString("ssid", DEFAULT_WIFI_SSID);
  wifiPassword = prefs.getString("pass", DEFAULT_WIFI_PASSWORD);
  prefs.end();
}

void saveWifiCredentials(const String &ssid, const String &password) {
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", password);
  prefs.end();
  wifiSsid = ssid;
  wifiPassword = password;
}

// Alarm persistence
AlarmConfig alarms[MAX_ALARMS];
int alarmCount = 0;

void loadAlarms() {
  prefs.begin("alarm", true);
  String encoded = prefs.getString("alarms", "");
  prefs.end();

  alarmCount = 0;
  if (encoded.length() == 0) {
    return;
  }

  // Format: enabled,hour,minute,sound[,schedule]|...
  // Older records without schedule default to daily.
  int start = 0;
  while (start < encoded.length() && alarmCount < MAX_ALARMS) {
    int sep = encoded.indexOf('|', start);
    if (sep < 0) sep = encoded.length();
    String chunk = encoded.substring(start, sep);

    int p0 = chunk.indexOf(',');
    int p1 = (p0 >= 0) ? chunk.indexOf(',', p0 + 1) : -1;
    int p2 = (p1 >= 0) ? chunk.indexOf(',', p1 + 1) : -1;
    int p3 = (p2 >= 0) ? chunk.indexOf(',', p2 + 1) : -1;

    if (p0 >= 0 && p1 >= 0 && p2 >= 0) {
      bool enabled = chunk.substring(0, p0) == "1";
      uint8_t hour = chunk.substring(p0 + 1, p1).toInt();
      uint8_t minute = chunk.substring(p1 + 1, p2).toInt();
      uint8_t sound = (p3 >= 0) ? chunk.substring(p2 + 1, p3).toInt() : chunk.substring(p2 + 1).toInt();
      uint8_t schedule = (p3 >= 0) ? chunk.substring(p3 + 1).toInt() : ALARM_SCHEDULE_DAILY;

      if (hour < 24 && minute < 60 && sound < ALARM_SOUND_COUNT && schedule < ALARM_SCHEDULE_COUNT) {
        alarms[alarmCount++] = {enabled, hour, minute, sound, schedule};
      }
    }

    start = sep + 1;
  }
}

void saveAlarms() {
  String encoded;
  for (int i = 0; i < alarmCount; i++) {
    if (i > 0) encoded += '|';
    encoded += alarms[i].enabled ? '1' : '0';
    encoded += ',';
    encoded += String(alarms[i].hour);
    encoded += ',';
    encoded += String(alarms[i].minute);
    encoded += ',';
    encoded += String(alarms[i].sound);
    encoded += ',';
    encoded += String(alarms[i].schedule);
  }

  prefs.begin("alarm", false);
  prefs.putString("alarms", encoded);
  prefs.end();
}

// WiFi mode tracking
bool wifiModeAP = false;  // true if running as Access Point, false if connected to external WiFi

// Pin definitions
#define BUZZER_PIN 20
#define DP_LED_PIN 10

// Buzzer timer configuration
#define BUZZER_CHANNEL 0
#define BUZZER_FREQUENCY 1000  // 1kHz
#define BUZZER_RESOLUTION 8    // 8-bit resolution
#define BUZZER_DUTY_CYCLE 128  // 50% duty cycle (128/256)

// I2C expander addresses
#define PCF8574N_ADDR_0 0x20  // A0=GND, A1=GND, A2=GND
#define PCF8574N_ADDR_1 0x21  // A0=VCC, A1=GND, A2=GND
#define PCF8574N_ADDR_2 0x22  // A0=GND, A1=VCC, A2=GND
#define PCF8574N_ADDR_3 0x23  // A0=VCC, A1=VCC, A2=GND
#define PCF8574N_ADDR_4 0x24  // A0=GND, A1=GND, A2=VCC

// Create PCF8574N instances with default addresses (adjust as needed)
PCF8574 expander_0(PCF8574N_ADDR_0, &Wire);
PCF8574 expander_1(PCF8574N_ADDR_1, &Wire);
PCF8574 expander_2(PCF8574N_ADDR_2, &Wire);
PCF8574 expander_3(PCF8574N_ADDR_3, &Wire);
PCF8574 expander_4(PCF8574N_ADDR_4, &Wire);

// Create ESP32 RTC instance
ESP32RTC rtc;

// Array of bytes representing time digits (0-9), 7 rows per digit
// This will be used to display time on the LED matrix
// Each byte represents a row of 5 pixels (bits 0-4), bits 5-7 are unused
// For example, digit '0' might look like this:
// 0b01110,  // Row 0: .###.
// 0b10001,  // Row 1: #...#
// 0b10001,  // Row 2: #...#
// 0b10001,  // Row 3: #...#
// 0b10001,  // Row 4: #...#
// 0b10001,  // Row 5: #...#
// 0b01110   // Row 6: .###.
// The actual bitmaps for digits 0-9 are defined in Digits5x8.cpp

// Array to store current time display (4 digits)
// Each element stores a pointer to the 8-byte bitmap for that digit
const uint8_t* timeDisplay[4] = {nullptr, nullptr, nullptr, nullptr};
uint8_t currentRow = 0; // Track current row being updated on the display
uint8_t rowPinValue = 0x01;

// Digit animation state
uint8_t currentDigits[4] = {0, 0, 0, 0};
uint8_t nextDigits[4] = {0, 0, 0, 0};
uint8_t animStep[4] = {DIGIT_HEIGHT, DIGIT_HEIGHT, DIGIT_HEIGHT, DIGIT_HEIGHT};
uint8_t animDelay[4] = {0, 0, 0, 0};
unsigned long lastAnimMillis = 0;
const unsigned long ANIM_STEP_MS = 80; // Animation frame duration in ms
const uint8_t ANIM_DELAY_STEPS = 2; // How many animation frames between each digit start

// Alarm state tracking
bool alarmTriggered[MAX_ALARMS] = {false};
int lastAlarmMinute = -1;

bool isAlarmScheduledForToday(uint8_t schedule, uint8_t dayOfWeek) {
  bool isWeekend = (dayOfWeek == 0 || dayOfWeek == 6);

  switch (schedule) {
    case ALARM_SCHEDULE_WEEKDAYS:
      return !isWeekend;
    case ALARM_SCHEDULE_WEEKENDS:
      return isWeekend;
    case ALARM_SCHEDULE_DAILY:
    default:
      return true;
  }
}

// Helper: return the row bytes for a digit position, applying slide-up animation when active
uint8_t getDigitRow(uint8_t pos, uint8_t row) {
  // If no active animation, just return the current digit row
  if (animStep[pos] >= DIGIT_HEIGHT) {
    return Digits5x8::getDigit(currentDigits[pos])[row];
  }

  const uint8_t* oldBmp = Digits5x8::getDigit(currentDigits[pos]);
  const uint8_t* newBmp = Digits5x8::getDigit(nextDigits[pos]);
  uint8_t t = animStep[pos];

  if (row + t < DIGIT_HEIGHT) {
    return oldBmp[row + t];
  }

  uint8_t newRow = row - (DIGIT_HEIGHT - t);
  if (newRow < DIGIT_HEIGHT) {
    return newBmp[newRow];
  }

  return 0;
}

// Buzzer control functions
void buzzerOn() {
  ledcWrite(BUZZER_CHANNEL, BUZZER_DUTY_CYCLE);
}

void buzzerOff() {
  ledcWrite(BUZZER_CHANNEL, 0);
}

void playNote(unsigned int frequency, unsigned long duration) {
  if (frequency == 0) {
    buzzerOff();
    delay(duration);
  } else {
    ledcWriteTone(BUZZER_CHANNEL, frequency);
    ledcWrite(BUZZER_CHANNEL, BUZZER_DUTY_CYCLE);
    delay(duration);
    buzzerOff();
  }
}

void playFanfare() {
  // Fanfare notes: C5, E5, G5, C6
  playNote(523, 200);  // C5
  delay(50);
  playNote(659, 200);  // E5
  delay(50);
  playNote(784, 200);  // G5
  delay(50);
  playNote(1047, 400); // C6
  delay(100);
}

void playSiren() {
  // Short siren sound for WiFi fallback activation
  // Rising siren: 800Hz to 1200Hz
  for (int freq = 800; freq <= 1200; freq += 50) {
    ledcWriteTone(BUZZER_CHANNEL, freq);
    ledcWrite(BUZZER_CHANNEL, BUZZER_DUTY_CYCLE);
    delay(30);
  }
  // Falling siren: 1200Hz to 800Hz
  for (int freq = 1200; freq >= 800; freq -= 50) {
    ledcWriteTone(BUZZER_CHANNEL, freq);
    ledcWrite(BUZZER_CHANNEL, BUZZER_DUTY_CYCLE);
    delay(30);
  }
  buzzerOff();
}

void beep(int count = 1) {
  for (int i = 0; i < count; i++) {
    playNote(1000, 100); // Beep at 1kHz for 100ms
    delay(100);
  }
}

void shortBeep() {
  playNote(1000, 10); // Beep at 1kHz for 100ms
  delay(100);
}

// WiFi connection function with AP fallback
bool connectToWiFi() {
  Serial.println("Attempting to connect to WiFi network...");

  // Ensure we have credentials loaded (fallback to defaults if not)
  if (wifiSsid.length() == 0) wifiSsid = DEFAULT_WIFI_SSID;
  if (wifiPassword.length() == 0) wifiPassword = DEFAULT_WIFI_PASSWORD;

  WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // 20 attempts = ~10 seconds
    delay(400);
    shortBeep();
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected to network!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiModeAP = false;
    return true;
  } else {
    Serial.println("\nWiFi network connection failed!");
    Serial.println("Starting Access Point mode as fallback...");

    WiFi.disconnect(true);
    delay(500);
    
    // Start Access Point mode
    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(AP_SSID, /*AP_PASSWORD*/NULL, 13, false, 4)) {
      Serial.println("Access Point started successfully!");
      Serial.print("AP SSID: ");
      Serial.println(AP_SSID);
      Serial.print("AP IP address: ");
      Serial.println(WiFi.softAPIP());
      wifiModeAP = true;
      playSiren();  // Play siren sound to indicate AP fallback mode
      return true;
    } else {
      Serial.println("Failed to start Access Point!");
      wifiModeAP = false;
      return false;
    }
  }
}

// Web server setup function
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/settime", HTTP_POST, handleApiSetTime);
  server.on("/api/wifi", HTTP_POST, handleApiWifi);
  server.on("/api/timezone", HTTP_POST, handleApiTimezone);
  server.on("/api/alarms", HTTP_POST, handleApiAlarms);
  server.onNotFound(handleNotFound);
}

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  delay(1000);
  
  // Setup GPIO
  pinMode(BUZZER_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(BUZZER_PIN, HIGH);  // Buzzer off (active LOW)
  pinMode(DP_LED_PIN, OUTPUT_OPEN_DRAIN);
  digitalWrite(DP_LED_PIN, HIGH);  // DP LED off

  // Setup buzzer timer
  ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY, BUZZER_RESOLUTION);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  delay(1000);  // Wait for peripherals to stabilize
  beep(1);    // Short beep to indicate startup
  
  // Load saved WiFi credentials (if any)
  loadWifiCredentials();

  // Load saved alarm settings (if any)
  loadAlarms();

  // Connect to WiFi
  if (connectToWiFi()) {
    // Setup web server routes
    setupWebServer();
    server.begin();
    Serial.println("Web server started");
    
    // Sync time with NTP only if connected to external network (not AP mode)
    if (!wifiModeAP) {
      Serial.println("Syncing time with NTP...");
      if (rtc.syncNTP("pool.ntp.org", getEffectiveGmtOffset(), 0)) {
        Serial.println("NTP sync successful!");
        playFanfare();
      } else {
        Serial.println("NTP sync failed, using current RTC time");
        beep(2);
      }
    } else {
      Serial.println("In AP mode - NTP sync not available. Use web interface to set time manually.");
      beep(2);
    }
  } else {
    Serial.println("WiFi connection failed, continuing without network features");
    beep(3);
  }
  
  // configure global Wire object for ESP32-C3 I2C pins SDA=GPIO9, SCL=GPIO8
  Wire.begin(8, 9); // SCL=GPIO9, SDA=GPIO8
  Wire.setClock(100000); // Set I2C clock frequency to 100kHz (standard mode)
  
  // Initialize I2C expanders and check connectivity
  bool allConnected = true;
  allConnected &= expander_0.begin();
  allConnected &= expander_1.begin();
  allConnected &= expander_2.begin();
  allConnected &= expander_3.begin();
  allConnected &= expander_4.begin();
  if (!allConnected) {
    beep(4);
    while(1);
  }
  
  // Initialize internal ESP32 RTC with current timezone
  if (!rtc.begin(getEffectiveGmtOffset() / 3600)) {  // Convert seconds to hours
    beep(5);
    while(1);
  }

  // Initialize display digits with current time (no animation on startup)
  {
    DateTime now = rtc.now();
    currentDigits[0] = now.hour / 10;
    currentDigits[1] = now.hour % 10;
    currentDigits[2] = now.minute / 10;
    currentDigits[3] = now.minute % 10;
    for (int i = 0; i < 4; i++) {
      nextDigits[i] = currentDigits[i];
      animStep[i] = DIGIT_HEIGHT;
    }
  }
  
  // Set RTC time manually if NTP failed (uncomment to set manually)
  // rtc.setDateTime(2026, 3, 12, 12, 0, 0);  // year, month, day, hour, min, sec
  
  // Set all expander_0 pins to LOW and other expanders to HIGH
  expander_0.write8(0x00); // All pins LOW
  expander_1.write8(0xFF); // All pins HIGH
  expander_2.write8(0xFF); // All pins HIGH
  expander_3.write8(0xFF); // All pins HIGH
  expander_4.write8(0xFF); // All pins HIGH
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  static unsigned long lastUpdate = 0;
  static unsigned long lastToggle = 0;
    
  // Display time update every 1 second
  if (millis() - lastUpdate > 1000) {
    DateTime now = rtc.now();

    char timeStr[9];
    rtc.getTimeString(timeStr);

    // Calculate new digit values and start animation where digits changed
    uint8_t newDigits[4] = {
      (uint8_t)(now.hour / 10),
      (uint8_t)(now.hour % 10),
      (uint8_t)(now.minute / 10),
      (uint8_t)(now.minute % 10)
    };

    for (int i = 0; i < 4; i++) {
      if (newDigits[i] != nextDigits[i]) {
        // Start animation for this digit (cascade with delay)
        currentDigits[i] = nextDigits[i];
        nextDigits[i] = newDigits[i];
        animStep[i] = 0;
        animDelay[i] = i * ANIM_DELAY_STEPS; // cascade delay based on digit position
      }
    }

    // Check alarms once per minute and trigger those that match
    int currentMinute = now.minute;
    if (currentMinute != lastAlarmMinute) {
      lastAlarmMinute = currentMinute;
      for (int i = 0; i < alarmCount; i++) {
        alarmTriggered[i] = false;
      }
    }

    for (int i = 0; i < alarmCount; i++) {
      if (!alarmTriggered[i] &&
          alarms[i].enabled &&
          isAlarmScheduledForToday(alarms[i].schedule, now.dayOfWeek) &&
          now.hour == alarms[i].hour &&
          now.minute == alarms[i].minute) {
        alarmTriggered[i] = true;

        switch (alarms[i].sound) {
          case ALARM_SOUND_BEEP:
            playNote(1000, 200);
            delay(100);
            playNote(1000, 200);
            delay(100);
            playNote(1000, 200);
            break;
          case ALARM_SOUND_MELODY:
            playFanfare();
            break;
          case ALARM_SOUND_SIREN:
          default:
            playSiren();
            break;
        }
      }
    }

    lastUpdate = millis();
  }

  // Advance animation stepframes at a fixed rate
  if (millis() - lastAnimMillis > ANIM_STEP_MS) {
    lastAnimMillis = millis();
    for (int i = 0; i < 4; i++) {
      if (animStep[i] < DIGIT_HEIGHT) {
        if (animDelay[i] > 0) {
          animDelay[i]--;
          continue;
        }
        animStep[i]++;
        if (animStep[i] >= DIGIT_HEIGHT) {
          // Animation finished: commit the new digit
          currentDigits[i] = nextDigits[i];
          animStep[i] = DIGIT_HEIGHT;
        }
      }
    }
  }

  // Toggle LED on pin 0 as heartbeat
  if (millis() - lastToggle > 500) {
    static bool ledState = false;
    digitalWrite(DP_LED_PIN, ledState ? LOW : HIGH);
    ledState = !ledState;
    lastToggle = millis();
  }

  // Set expanders 1-4 to the values from the display array. Each digit is one expander. Low is active.
  expander_0.write8(0x00); // Row control pins (active LOW)
  expander_1.write8(~getDigitRow(0, currentRow)); // Digit 0
  expander_2.write8(~getDigitRow(1, currentRow)); // Digit 1
  expander_3.write8(~getDigitRow(2, currentRow)); // Digit 2
  expander_4.write8(~getDigitRow(3, currentRow)); // Digit 3
  expander_0.write8(rowPinValue);
  // delay(1); // Short delay to allow display to update

  rowPinValue <<= 1;
  if (rowPinValue == 0x80) {
    rowPinValue = 0x00; // After the last row, set to 0 to turn off all rows before resetting to 0x01
  }
  currentRow++;
  if (currentRow >= DIGIT_HEIGHT) {
    currentRow = 0;
    rowPinValue = 0x01;
  }
  
}
