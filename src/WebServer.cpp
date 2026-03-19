/**
 * @file WebServer.cpp
 * @brief HTTP handlers for the LED clock single-page web interface.
 *
 * The web layer shares the same compact alarm record format as the persistence
 * layer in `main.cpp`. Titles are percent-decoded here so the browser can send
 * them safely inside a comma- and pipe-delimited payload.
 */

#include "WebServerHandlers.h"
#include "WebRoot.h"
#include "AlarmMelodies.h"
#include <Arduino.h>
#include <WiFi.h>

struct DigitFontOption {
  Digits5x8::FontStyle value;
  const char* id;
  const char* name;
};

static const DigitFontOption DIGIT_FONT_OPTIONS[] = {
  {Digits5x8::FontStyle::Classic, "classic", "Classic"},
  {Digits5x8::FontStyle::HD44780, "hd44780", "HD44780"}
};

static const size_t DIGIT_FONT_COUNT = sizeof(DIGIT_FONT_OPTIONS) / sizeof(DIGIT_FONT_OPTIONS[0]);

struct SnoozeActionOption {
  SnoozeButtonAction value;
  const char* id;
  const char* name;
};

static const SnoozeActionOption SNOOZE_ACTION_OPTIONS[] = {
  {SNOOZE_BUTTON_ACTION_SNOOZE, "snooze", "Snooze alarm"},
  {SNOOZE_BUTTON_ACTION_DISMISS, "dismiss", "Stop alarm"}
};

static const size_t SNOOZE_ACTION_COUNT = sizeof(SNOOZE_ACTION_OPTIONS) / sizeof(SNOOZE_ACTION_OPTIONS[0]);

static const DigitFontOption& currentDigitFontOption() {
  Digits5x8::FontStyle current = Digits5x8::getFont();
  for (size_t i = 0; i < DIGIT_FONT_COUNT; i++) {
    if (DIGIT_FONT_OPTIONS[i].value == current) {
      return DIGIT_FONT_OPTIONS[i];
    }
  }
  return DIGIT_FONT_OPTIONS[0];
}

/**
 * @brief Sends a JSON response with the given HTTP status.
 * @param code HTTP status code.
 * @param payload Serialized JSON body.
 */
static void sendJson(int code, const String &payload) {
  server.send(code, "application/json", payload);
}

/**
 * @brief Escapes a string for inclusion in JSON output.
 * @param s Raw input string.
 * @return JSON-safe string content without surrounding quotes.
 */
static String jsonEscape(const String &s) {
  String out;
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    switch (c) {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: out += c; break;
    }
  }
  return out;
}

/**
 * @brief Converts one hexadecimal digit to its numeric value.
 * @param c ASCII hex digit.
 * @return Nibble value or `-1` if invalid.
 */
static int fromHexDigit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;
}

/**
 * @brief Decodes a percent-encoded alarm title from the web payload.
 * @param encoded Encoded title segment.
 * @return Decoded title with surrounding whitespace trimmed.
 */
static String decodeAlarmTitle(const String &encoded) {
  String decoded;
  decoded.reserve(encoded.length());

  for (size_t i = 0; i < encoded.length(); i++) {
    char c = encoded[i];
    if (c == '%' && i + 2 < encoded.length()) {
      int hi = fromHexDigit(encoded[i + 1]);
      int lo = fromHexDigit(encoded[i + 2]);
      if (hi >= 0 && lo >= 0) {
        decoded += static_cast<char>((hi << 4) | lo);
        i += 2;
        continue;
      }
    }
    decoded += c;
  }

  decoded.trim();
  return decoded;
}

/**
 * @brief Serves the main single-page web interface.
 */
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

void handleSetTime() {
  String html = "<html><head><title>Set Time</title></head><body>";
  html += "<h1>Set Clock Time</h1>";
  html += "<form action='/settime' method='POST'>";
  html += "Hour: <input type='number' name='hour' min='0' max='23' required><br>";
  html += "Minute: <input type='number' name='minute' min='0' max='59' required><br>";
  html += "Second: <input type='number' name='second' min='0' max='59' required><br>";
  html += "<input type='submit' value='Set Time'>";
  html += "</form>";
  html += "<p><a href='/'>Back</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleSetTimePost() {
  if (server.hasArg("hour") && server.hasArg("minute") && server.hasArg("second")) {
    int hour = server.arg("hour").toInt();
    int minute = server.arg("minute").toInt();
    int second = server.arg("second").toInt();
    
    // Get current date and set new time
    DateTime now = rtc.now();
    rtc.setDateTime(now.year, now.month, now.day, hour, minute, second);
    
    server.send(200, "text/html", "<html><body><h1>Time Set Successfully!</h1><p><a href='/'>Back to Home</a></p></body></html>");
    // Audio feedback is handled by shared melody helpers.
    // The caller should handle audio feedback
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: Missing parameters</h1><p><a href='/settime'>Try again</a></p></body></html>");
  }
}
/**
 * @brief Returns the current device state, including alarms, as JSON.
 *
 * @details
 * Alarm objects are expanded into named JSON fields here even though they are
 * stored compactly in NVS. This keeps the browser code straightforward while
 * allowing the firmware to retain its compact persistence format.
 */
void handleApiStatus() {
  DateTime now = rtc.now();
  char timeStr[20];
  rtc.getTimeString(timeStr);

  String tzList = "[";
  for (int i = 0; i < NUM_TIMEZONES; i++) {
    tzList += "{\"index\":" + String(i);
    tzList += ",\"name\":\"" + jsonEscape(timezones[i].name) + "\"";
    tzList += ",\"description\":\"" + jsonEscape(timezones[i].description) + "\"}";
    if (i < NUM_TIMEZONES - 1) tzList += ",";
  }
  tzList += "]";

  String fontList = "[";
  for (size_t i = 0; i < DIGIT_FONT_COUNT; i++) {
    fontList += "{\"index\":" + String(static_cast<uint8_t>(DIGIT_FONT_OPTIONS[i].value));
    fontList += ",\"id\":\"" + jsonEscape(String(DIGIT_FONT_OPTIONS[i].id)) + "\"";
    fontList += ",\"name\":\"" + jsonEscape(String(DIGIT_FONT_OPTIONS[i].name)) + "\"}";
    if (i + 1 < DIGIT_FONT_COUNT) fontList += ",";
  }
  fontList += "]";

  const DigitFontOption &currentFont = currentDigitFontOption();

  String snoozeActionList = "[";
  for (size_t i = 0; i < SNOOZE_ACTION_COUNT; i++) {
    snoozeActionList += "{\"index\":" + String(static_cast<uint8_t>(SNOOZE_ACTION_OPTIONS[i].value));
    snoozeActionList += ",\"id\":\"" + jsonEscape(String(SNOOZE_ACTION_OPTIONS[i].id)) + "\"";
    snoozeActionList += ",\"name\":\"" + jsonEscape(String(SNOOZE_ACTION_OPTIONS[i].name)) + "\"}";
    if (i + 1 < SNOOZE_ACTION_COUNT) snoozeActionList += ",";
  }
  snoozeActionList += "]";

  String json = "{";
  json += "\"success\":true,";
  json += "\"time\":\"" + jsonEscape(String(timeStr)) + "\",";
  json += "\"hour\":" + String(now.hour) + ",";
  json += "\"minute\":" + String(now.minute) + ",";
  json += "\"second\":" + String(now.second) + ",";
  json += "\"timezoneIndex\":" + String(currentTimezoneIndex) + ",";
  json += "\"timezone\":{";
  json += "\"name\":\"" + jsonEscape(timezones[currentTimezoneIndex].name) + "\",";
  json += "\"description\":\"" + jsonEscape(timezones[currentTimezoneIndex].description) + "\"}";
  json += ",\"fontIndex\":" + String(static_cast<uint8_t>(currentFont.value));
  json += ",\"font\":{";
  json += "\"id\":\"" + jsonEscape(String(currentFont.id)) + "\",";
  json += "\"name\":\"" + jsonEscape(String(currentFont.name)) + "\"}";
  json += ",\"fonts\":" + fontList;
  json += ",\"snoozeButtonActions\":" + snoozeActionList;
  json += ",\"dstMode\":\"" + String(dstMode == DST_AUTO ? "auto" : "manual") + "\"";
  json += ",\"manualDstOffset\":" + String(manualDstOffset);
  json += ",\"timezones\":" + tzList;
  json += ",\"wifiMode\":\"" + String(wifiModeAP ? "AP" : "station") + "\"";
  json += ",\"wifiSsid\":\"" + jsonEscape(wifiSsid) + "\"";
  json += ",\"apSsid\":\"" + jsonEscape(String(AP_SSID)) + "\"";
  json += ",\"alarms\": [";
  for (int i = 0; i < alarmCount; i++) {
    json += "{";
    json += "\"enabled\":" + String(alarms[i].enabled ? "true" : "false");
    json += ",\"hour\":" + String(alarms[i].hour);
    json += ",\"minute\":" + String(alarms[i].minute);
    json += ",\"sound\":" + String(alarms[i].sound);
    json += ",\"schedule\":" + String(alarms[i].schedule);
    json += ",\"buttonAction\":" + String(alarms[i].buttonAction);
    json += ",\"snoozeMinutes\":" + String(alarms[i].snoozeMinutes);
    json += ",\"title\":\"" + jsonEscape(alarms[i].title) + "\"";
    json += "}";
    if (i < alarmCount - 1) json += ",";
  }
  json += "]";

  if (wifiModeAP) {
    json += ",\"ip\":\"" + jsonEscape(WiFi.softAPIP().toString()) + "\"";
  } else {
    json += ",\"ip\":\"" + jsonEscape(WiFi.localIP().toString()) + "\"";
  }
  json += "}";

  sendJson(200, json);
}

void handleApiSetTime() {
  if (server.hasArg("hour") && server.hasArg("minute") && server.hasArg("second")) {
    int hour = server.arg("hour").toInt();
    int minute = server.arg("minute").toInt();
    int second = server.arg("second").toInt();

    DateTime now = rtc.now();
    rtc.setDateTime(now.year, now.month, now.day, hour, minute, second);
    playFanfare();

    sendJson(200, "{\"success\":true,\"message\":\"Time updated\"}");
  } else {
    sendJson(400, "{\"success\":false,\"message\":\"Missing parameters\"}");
  }
}

void handleApiWifi() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String newSSID = server.arg("ssid");
    String newPassword = server.arg("password");

    Serial.print("New SSID: ");
    Serial.println(newSSID);
    Serial.print("New Password: ");
    Serial.println(newPassword);

    // Persist credentials to NVS so they remain across reboots
    saveWifiCredentials(newSSID, newPassword);

    Serial.println("Credentials saved, restarting");
    sendJson(200, "{\"success\":true,\"message\":\"Saved - restarting\"}");
    delay(2000);
    ESP.restart();
  } else {
    Serial.println("Missing SSID or password");
    sendJson(400, "{\"success\":false,\"message\":\"Invalid request\"}");
  }
}

/**
 * @brief Saves the full alarm list received from the browser.
 *
 * @details
 * The browser submits alarms in one compact form field:
 * `enabled,hour,minute,sound,schedule,buttonAction,snoozeMinutes,title|...`
 *
 * Titles are percent-encoded so commas and pipes remain reserved as field and
 * record delimiters. The parser requires the full eight-field record layout and
 * skips invalid records rather than accepting partially corrupt data.
 */
void handleApiAlarms() {
  if (!server.hasArg("alarms")) {
    sendJson(400, "{\"success\":false,\"message\":\"Missing alarms parameter\"}");
    return;
  }

  String encoded = server.arg("alarms");
  if (encoded.length() == 0) {
    alarmCount = 0;
    saveAlarms();
    sendJson(200, "{\"success\":true,\"message\":\"Alarms cleared\"}");
    return;
  }

  // Parse one record at a time so bad data cannot poison the whole list.
  int newCount = 0;
  int start = 0;
  while (start < encoded.length() && newCount < MAX_ALARMS) {
    int sep = encoded.indexOf('|', start);
    if (sep < 0) sep = encoded.length();
    String chunk = encoded.substring(start, sep);

    int p0 = chunk.indexOf(',');
    int p1 = (p0 >= 0) ? chunk.indexOf(',', p0 + 1) : -1;
    int p2 = (p1 >= 0) ? chunk.indexOf(',', p1 + 1) : -1;
    int p3 = (p2 >= 0) ? chunk.indexOf(',', p2 + 1) : -1;
    int p4 = (p3 >= 0) ? chunk.indexOf(',', p3 + 1) : -1;
    int p5 = (p4 >= 0) ? chunk.indexOf(',', p4 + 1) : -1;
    int p6 = (p5 >= 0) ? chunk.indexOf(',', p5 + 1) : -1;

    if (p0 >= 0 && p1 >= 0 && p2 >= 0 && p3 >= 0 && p4 >= 0 && p5 >= 0 && p6 >= 0) {
      bool enabled = chunk.substring(0, p0) == "1";
      uint8_t hour = chunk.substring(p0 + 1, p1).toInt();
      uint8_t minute = chunk.substring(p1 + 1, p2).toInt();
      uint8_t sound = chunk.substring(p2 + 1, p3).toInt();
      uint8_t schedule = chunk.substring(p3 + 1, p4).toInt();
      uint8_t buttonAction = chunk.substring(p4 + 1, p5).toInt();
      uint8_t snoozeMinutes = chunk.substring(p5 + 1, p6).toInt();
      String title = decodeAlarmTitle(chunk.substring(p6 + 1));

      if (hour < 24 &&
          minute < 60 &&
          sound < ALARM_SOUND_COUNT &&
          schedule < ALARM_SCHEDULE_COUNT &&
          buttonAction < SNOOZE_BUTTON_ACTION_COUNT &&
          snoozeMinutes >= 1 &&
          snoozeMinutes <= 60) {
        if (title.length() == 0) {
          title = "Alarm " + String(newCount + 1);
        }
        alarms[newCount++] = {enabled, hour, minute, sound, schedule, buttonAction, snoozeMinutes, title};
      }
    }

    start = sep + 1;
  }

  alarmCount = newCount;
  saveAlarms();
  sendJson(200, "{\"success\":true,\"message\":\"Alarms saved\"}");
}

void handleApiAlarmTest() {
  if (!server.hasArg("sound")) {
    sendJson(400, "{\"success\":false,\"message\":\"Missing sound parameter\"}");
    return;
  }

  int sound = server.arg("sound").toInt();
  if (sound < 0 || sound >= ALARM_SOUND_COUNT) {
    sendJson(400, "{\"success\":false,\"message\":\"Invalid sound\"}");
    return;
  }

  playAlarmSound(static_cast<uint8_t>(sound));
  sendJson(200, "{\"success\":true,\"message\":\"Sound played\"}");
}

void handleApiTimezone() {
  if (server.hasArg("timezone") && server.hasArg("dstmode")) {
    int newTimezoneIndex = server.arg("timezone").toInt();
    String dstModeStr = server.arg("dstmode");

    if (newTimezoneIndex >= 0 && newTimezoneIndex < NUM_TIMEZONES) {
      currentTimezoneIndex = newTimezoneIndex;

      if (dstModeStr == "auto") {
        dstMode = DST_AUTO;
      } else {
        dstMode = DST_MANUAL;
        if (server.hasArg("manualdst")) {
          manualDstOffset = server.arg("manualdst").toInt();
        }
      }

      // Reinitialize RTC with the new timezone
      rtc.begin(getEffectiveGmtOffset() / 3600);

      // Sync NTP if network is available
      bool ntpSuccess = false;
      if (!wifiModeAP && WiFi.status() == WL_CONNECTED) {
        ntpSuccess = rtc.syncNTP("pool.ntp.org", getEffectiveGmtOffset(), getEffectiveDstOffset());
      }

      if (ntpSuccess) {
        playFanfare();
      } else {
        beep(1);
      }

      sendJson(200, "{\"success\":true,\"message\":\"Timezone updated\"}");
    } else {
      sendJson(400, "{\"success\":false,\"message\":\"Invalid timezone\"}");
    }
  } else {
    sendJson(400, "{\"success\":false,\"message\":\"Missing parameters\"}");
  }
}

void handleApiSettings() {
  if (!server.hasArg("font")) {
    sendJson(400, "{\"success\":false,\"message\":\"Missing parameters\"}");
    return;
  }

  int fontIndex = server.arg("font").toInt();

  if (fontIndex < 0 || fontIndex >= static_cast<int>(DIGIT_FONT_COUNT)) {
    sendJson(400, "{\"success\":false,\"message\":\"Invalid settings\"}");
    return;
  }

  saveDigitFont(static_cast<Digits5x8::FontStyle>(fontIndex));
  sendJson(200, "{\"success\":true,\"message\":\"Settings updated\"}");
}

void handleNotFound() {
  Serial.print("Not found request: ");
  Serial.println(server.uri());
  if (server.uri().startsWith("/api/")) {
    sendJson(404, "{\"success\":false,\"message\":\"Not found\"}");
  } else {
    handleRoot();
  }
}

void handleFavicon() {
  server.send(204); // No Content
}
void handleWiFiSettings() {
  String html = "<html><head><title>WiFi Settings</title></head><body>";
  html += "<h1>WiFi Configuration</h1>";
  
  if (wifiModeAP) {
    html += "<p><strong>Currently in Access Point mode</strong></p>";
    html += "<p>Configure your WiFi credentials below. The clock will restart and try to connect to your network.</p>";
    html += "<form action='/wifi' method='POST'>";
    html += "WiFi SSID: <input type='text' name='ssid' required><br>";
    html += "WiFi Password: <input type='password' name='password' required><br>";
    html += "<input type='submit' value='Save and Restart'>";
    html += "</form>";
  } else {
    html += "<p><strong>Currently connected to: " + wifiSsid + "</strong></p>";
    html += "<p>If you need to change WiFi settings, restart the device in AP mode by holding a button or power cycling while WiFi is unavailable.</p>";
  }
  
  html += "<p><a href='/'>Back to Home</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleWiFiSettingsPost() {
  if (wifiModeAP && server.hasArg("ssid") && server.hasArg("password")) {
    String newSSID = server.arg("ssid");
    String newPassword = server.arg("password");
    
    // Note: In a real implementation, you'd save these to EEPROM/Preferences
    // For now, just show confirmation
    String html = "<html><body><h1>WiFi Settings Saved!</h1>";
    html += "<p>New SSID: " + newSSID + "</p>";
    html += "<p><strong>Note:</strong> Settings are not persisted. To make them permanent, modify the code constants.</p>";
    html += "<p>The device will restart in 5 seconds...</p>";
    html += "</body></html>";
    
    server.send(200, "text/html", html);
    
    // Restart after a delay
    delay(5000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: Invalid request</h1><p><a href='/wifi'>Try again</a></p></body></html>");
  }
}

void handleTimezoneSettings() {
  String html = "<html><head><title>Timezone Settings</title></head><body>";
  html += "<h1>Timezone Configuration</h1>";
  html += "<p>Current timezone: <strong>" + String(timezones[currentTimezoneIndex].name) + " - " + String(timezones[currentTimezoneIndex].description) + "</strong></p>";
  html += "<p>Current DST mode: <strong>" + String(dstMode == DST_AUTO ? "Automatic" : "Manual") + "</strong></p>";
  if (dstMode == DST_MANUAL) {
    html += "<p>Manual DST offset: <strong>" + String(manualDstOffset / 3600) + " hours</strong></p>";
  }
  
  html += "<form action='/timezone' method='POST'>";
  html += "<h3>Timezone Selection:</h3>";
  html += "<select name='timezone' required>";
  
  for (int i = 0; i < 10; i++) {  // NUM_TIMEZONES is not accessible here, using 10
    html += "<option value='" + String(i) + "'";
    if (i == currentTimezoneIndex) {
      html += " selected";
    }
    html += ">" + String(timezones[i].name) + " - " + String(timezones[i].description) + "</option>";
  }
  
  html += "</select><br><br>";
  
  html += "<h3>DST (Daylight Saving Time) Mode:</h3>";
  html += "<input type='radio' name='dstmode' value='auto'";
  if (dstMode == DST_AUTO) html += " checked";
  html += "> Automatic (use timezone rules)<br>";
  html += "<input type='radio' name='dstmode' value='manual'";
  if (dstMode == DST_MANUAL) html += " checked";
  html += "> Manual<br>";
  
  html += "<div id='manual-dst' style='margin-left: 20px;";
  if (dstMode != DST_MANUAL) html += " display: none;";
  html += "'>";
  html += "DST Offset: <select name='manualdst'>";
  html += "<option value='0'" + String(manualDstOffset == 0 ? " selected" : "") + ">No DST (0 hours)</option>";
  html += "<option value='3600'" + String(manualDstOffset == 3600 ? " selected" : "") + ">+1 hour</option>";
  html += "<option value='7200'" + String(manualDstOffset == 7200 ? " selected" : "") + ">+2 hours</option>";
  html += "</select><br>";
  html += "</div>";
  
  html += "<br><input type='submit' value='Update Settings and Sync NTP'>";
  html += "</form>";
  
  html += "<script>";
  html += "function toggleManualDST() {";
  html += "  var radios = document.getElementsByName('dstmode');";
  html += "  var manualDiv = document.getElementById('manual-dst');";
  html += "  for (var i = 0; i < radios.length; i++) {";
  html += "    if (radios[i].checked && radios[i].value === 'manual') {";
  html += "      manualDiv.style.display = 'block';";
  html += "    } else if (radios[i].checked && radios[i].value === 'auto') {";
  html += "      manualDiv.style.display = 'none';";
  html += "    }";
  html += "  }";
  html += "}";
  html += "var radios = document.getElementsByName('dstmode');";
  html += "for (var i = 0; i < radios.length; i++) {";
  html += "  radios[i].onchange = toggleManualDST;";
  html += "}";
  html += "</script>";
  
  html += "<p><a href='/'>Back to Home</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleTimezoneSettingsPost() {
  if (server.hasArg("timezone") && server.hasArg("dstmode")) {
    int newTimezoneIndex = server.arg("timezone").toInt();
    String dstModeStr = server.arg("dstmode");
    
    if (newTimezoneIndex >= 0 && newTimezoneIndex < 10) {  // Using 10 instead of NUM_TIMEZONES
      currentTimezoneIndex = newTimezoneIndex;
      
      // Update DST mode
      if (dstModeStr == "auto") {
        dstMode = DST_AUTO;
      } else if (dstModeStr == "manual") {
        dstMode = DST_MANUAL;
        if (server.hasArg("manualdst")) {
          manualDstOffset = server.arg("manualdst").toInt();
        }
      }
      
      // Note: RTC reinitialization and NTP sync should be handled by the caller
      // since we don't have access to getEffectiveGmtOffset() and getEffectiveDstOffset() here
      
      String html = "<html><body><h1>Timezone Settings Updated!</h1>";
      html += "<p>New timezone: <strong>" + String(timezones[currentTimezoneIndex].name) + "</strong></p>";
      html += "<p>DST mode: <strong>" + String(dstMode == DST_AUTO ? "Automatic" : "Manual");
      if (dstMode == DST_MANUAL) {
        html += " (" + String(manualDstOffset / 3600) + " hours offset)";
      }
      html += "</strong></p>";
      html += "<p>Settings applied. NTP sync may be required.</p>";
      html += "<p><a href='/'>Back to Home</a></p>";
      html += "</body></html>";
      
      server.send(200, "text/html", html);
      
      // Note: Audio feedback should be handled by the caller
    } else {
      server.send(400, "text/html", "<html><body><h1>Error: Invalid timezone</h1><p><a href='/timezone'>Try again</a></p></body></html>");
    }
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: Missing parameters</h1><p><a href='/timezone'>Try again</a></p></body></html>");
  }
}
