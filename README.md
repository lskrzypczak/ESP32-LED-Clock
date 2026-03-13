# ESP32 LED Clock with WiFi

This is an ESP32-based LED clock project that displays time using PCF8574 I2C expanders and includes WiFi connectivity for NTP time synchronization and web-based configuration.

## Features

- 4-digit 7-segment LED display using PCF8574 I2C expanders
- ESP32 internal RTC for timekeeping
- WiFi connectivity with NTP synchronization
- Web server for time configuration and status monitoring
- Buzzer for audio feedback
- Heartbeat LED indicator

## Hardware Requirements

- ESP32-C3 Mini or compatible board
- 5x PCF8574 I2C expanders (addresses 0x20-0x24)
- LED matrix display (4 digits, 5x8 pixels each)
- Buzzer and status LED
- Appropriate power supply

## WiFi Configuration

Before uploading the code, you need to configure your WiFi credentials:

1. Open `src/main.cpp`
2. Find these lines near the top:
   ```cpp
   const char* WIFI_SSID = "YourWiFiSSID";        // Replace with your WiFi SSID
   const char* WIFI_PASSWORD = "YourWiFiPassword"; // Replace with your WiFi password
   ```
3. Replace `"YourWiFiSSID"` with your actual WiFi network name
4. Replace `"YourWiFiPassword"` with your actual WiFi password

### Access Point Fallback

If the configured WiFi network is not available, the clock will automatically start as an Access Point with these settings:
- **AP SSID**: `LED-Clock-AP`
- **AP Password**: `clock1234`
- **AP IP**: `192.168.4.1`

In AP mode, you can:
- Connect to the AP network from your phone/computer
- Access the web interface at `http://192.168.4.1`
- Configure WiFi settings through the web interface
- Manually set the time

## Web Interface

Once connected to WiFi (or AP), the clock provides a web interface:

- **Home page** (`/`): Shows current time, timezone, DST status, WiFi mode, and connection details
- **Set Time** (`/settime`): Manual time setting interface
- **Timezone Settings** (`/timezone`): Configure timezone and DST mode (automatic or manual)
- **WiFi Settings** (`/wifi`): Configure WiFi credentials (only available in AP mode)

### DST (Daylight Saving Time) Configuration

The clock supports two DST modes:
- **Automatic**: Uses built-in DST rules for the selected timezone
- **Manual**: Allows manual control of DST offset (0, +1, or +2 hours)

In manual mode, you can override the automatic DST behavior for special cases or regions not covered by automatic rules.

### Supported Timezones

The clock supports the following timezones:
- UTC: Coordinated Universal Time
- CET/CEST: Central European Time with DST
- EST/EDT: Eastern Time with DST
- PST/PDT: Pacific Time with DST
- GMT/BST: Greenwich Mean Time with DST
- JST: Japan Standard Time

**Note**: Timezone and DST settings are not currently persisted across device restarts. The device defaults to CET with automatic DST on startup.

## NTP Synchronization

The clock automatically synchronizes time with NTP servers when WiFi is connected. The timezone and DST settings can be configured through the web interface. DST is handled automatically using proper timezone rules for CET/CEST, with manual override available for special cases.

## Building and Uploading

1. Configure WiFi credentials as described above
2. Build the project: `pio run`
3. Upload to ESP32: `pio run --target upload`
4. Monitor serial output: `pio device monitor`

## Audio Feedback

- **Startup**: 1 beep to indicate device startup
- **WiFi Connected**: Fanfare melody when NTP sync succeeds (station mode)
- **AP Mode Activated**: Siren sound when Access Point fallback is activated
- **AP Mode Info**: 2 beeps when starting in Access Point mode (without NTP)
- **WiFi/AP Failure**: 3 beeps if both station and AP modes fail
- **RTC Failure**: 2 beeps
- **I2C Failure**: 1 beep
- **Time Set**: Fanfare confirmation when time is set via web interface

## Pin Configuration

- I2C: SDA=GPIO8, SCL=GPIO9
- Buzzer: GPIO20
- Status LED: GPIO10
- PCF8574 addresses: 0x20, 0x21, 0x22, 0x23, 0x24

## Troubleshooting

- If WiFi connection fails, the clock will automatically start in Access Point mode
- In AP mode, connect to `LED-Clock-AP` network with password `clock1234`
- Access the web interface at `http://192.168.4.1` to configure WiFi settings
- Check serial output for connection status and IP address
- Ensure PCF8574 addresses match your hardware setup
- Verify I2C wiring and pull-up resistors