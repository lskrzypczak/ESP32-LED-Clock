#include "ESP32_RTC.h"

ESP32RTC::ESP32RTC() : _timezone(0), _lastMillis(0), _secondOffset(0) {
}

bool ESP32RTC::begin(int8_t timezone) {
    _timezone = timezone;
    _lastMillis = millis();
    
    // Initialize system time (internal RTC is always running on ESP32)
    return true;
}

DateTime ESP32RTC::now() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    
    DateTime dt;
    dt.year = timeinfo->tm_year + 1900;
    dt.month = timeinfo->tm_mon + 1;
    dt.day = timeinfo->tm_mday;
    dt.hour = timeinfo->tm_hour;
    dt.minute = timeinfo->tm_min;
    dt.second = timeinfo->tm_sec;
    dt.dayOfWeek = timeinfo->tm_wday;
    
    return dt;
}

void ESP32RTC::setDateTime(uint16_t year, uint8_t month, uint8_t day,
                           uint8_t hour, uint8_t minute, uint8_t second) {
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    timeinfo.tm_isdst = -1;
    
    time_t t = mktime(&timeinfo);
    struct timeval tv = {.tv_sec = t, .tv_usec = 0};
    settimeofday(&tv, NULL);
}

void ESP32RTC::setTimezone(int8_t timezone) {
    _timezone = timezone;
}

bool ESP32RTC::syncNTP(const char* ntpServer, long gmtOffset_sec, long daylightOffset_sec) {
    // Set timezone environment variable for proper DST handling
    // Currently hardcoded for CET/CEST. For other timezones, this would need to be made dynamic.
    // Format: CET-1CEST,M3.5.0,M10.5.0/3
    // CET-1: Standard time is CET (UTC+1)
    // CEST: DST time is CEST (UTC+2) 
    // M3.5.0: DST starts on last Sunday in March
    // M10.5.0/3: DST ends on last Sunday in October at 3:00
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    
    // Use 0 for daylightOffset_sec since TZ handles DST
    configTime(gmtOffset_sec, 0, ntpServer);
    
    // Wait for time to be set
    time_t now = time(nullptr);
    uint8_t attempts = 0;
    while (now < 24 * 3600 && attempts < 20) {
        delay(500);
        now = time(nullptr);
        attempts++;
    }
    
    return (now > 24 * 3600);  // Check if time is valid (after 1970)
}

uint32_t ESP32RTC::getUnixTime() {
    return time(nullptr);
}

void ESP32RTC::setUnixTime(uint32_t timestamp) {
    struct timeval tv = {.tv_sec = timestamp, .tv_usec = 0};
    settimeofday(&tv, NULL);
}

void ESP32RTC::getTimeString(char* buffer) {
    DateTime dt = now();
    sprintf(buffer, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
}

void ESP32RTC::getDateString(char* buffer) {
    DateTime dt = now();
    sprintf(buffer, "%04d-%02d-%02d", dt.year, dt.month, dt.day);
}

void ESP32RTC::getDateTimeString(char* buffer) {
    DateTime dt = now();
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            dt.year, dt.month, dt.day,
            dt.hour, dt.minute, dt.second);
}

uint16_t ESP32RTC::getMilliseconds() {
    return millis() % 1000;
}
