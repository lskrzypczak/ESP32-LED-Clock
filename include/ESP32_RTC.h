#ifndef ESP32_RTC_H
#define ESP32_RTC_H

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>

struct DateTime {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t dayOfWeek;  // 0 = Sunday, 1 = Monday, etc.
};

class ESP32RTC {
public:
    /**
     * @brief Constructor for ESP32 internal RTC
     */
    ESP32RTC();

    /**
     * @brief Initialize the RTC
     * @param timezone Timezone offset from UTC (e.g., 1 for CET)
     * @return true if initialization successful
     */
    bool begin(int8_t timezone = 0);

    /**
     * @brief Get current date and time
     * @return DateTime struct with current values
     */
    DateTime now();

    /**
     * @brief Set date and time
     * @param year Year (2000+)
     * @param month Month (1-12)
     * @param day Day (1-31)
     * @param hour Hour (0-23)
     * @param minute Minute (0-59)
     * @param second Second (0-59)
     */
    void setDateTime(uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second);

    /**
     * @brief Set timezone offset from UTC
     * @param timezone Hours offset (e.g., 1 for CET, -5 for EST)
     */
    void setTimezone(int8_t timezone);

    /**
     * @brief Synchronize time from NTP server (requires WiFi)
     * @param ntpServer NTP server address (default: "pool.ntp.org")
     * @param gmtOffset_sec Timezone offset in seconds
     * @param daylightOffset_sec Daylight saving offset in seconds
     * @return true if synchronization successful
     */
    bool syncNTP(const char* ntpServer = "pool.ntp.org",
                 long gmtOffset_sec = 0, long daylightOffset_sec = 0);

    /**
     * @brief Get Unix timestamp (seconds since 1970-01-01 UTC)
     * @return Unix timestamp
     */
    uint32_t getUnixTime();

    /**
     * @brief Set Unix timestamp
     * @param timestamp Unix timestamp
     */
    void setUnixTime(uint32_t timestamp);

    /**
     * @brief Format current time as string (HH:MM:SS)
     * @param buffer Buffer to store formatted string (min 9 chars)
     */
    void getTimeString(char* buffer);

    /**
     * @brief Format current date as string (YYYY-MM-DD)
     * @param buffer Buffer to store formatted string (min 11 chars)
     */
    void getDateString(char* buffer);

    /**
     * @brief Format full date and time (YYYY-MM-DD HH:MM:SS)
     * @param buffer Buffer to store formatted string (min 20 chars)
     */
    void getDateTimeString(char* buffer);

    /**
     * @brief Get milliseconds since last second
     * @return Milliseconds (0-999)
     */
    uint16_t getMilliseconds();

private:
    int8_t _timezone;
    uint32_t _lastMillis;
    uint32_t _secondOffset;
};

#endif // ESP32_RTC_H
