#ifndef DIGITS5X8_H
#define DIGITS5X8_H

#include <Arduino.h>

// 5x8 bitmap font for digits 0-9
// Each digit is 8 bytes representing rows 0-7 (top to bottom)
// Each byte represents columns 0-4 (left to right)
#define DIGIT_WIDTH 5
#define DIGIT_HEIGHT 8

class Digits5x8 {
public:
    enum class FontStyle : uint8_t {
        Classic,
        HD44780
    };

    /**
     * @brief Get bitmap data for a digit
     * @param digit Digit 0-9
     * @return Pointer to 5-byte array containing bitmap data
     */
    static const uint8_t* getDigit(uint8_t digit);
    static const uint8_t* getDigit(uint8_t digit, FontStyle font);
    static void setFont(FontStyle font);
    static FontStyle getFont();

private:
    // Classic custom digit bitmaps - 5x8 pixels each
    static const uint8_t classicDigit0[DIGIT_HEIGHT];
    static const uint8_t classicDigit1[DIGIT_HEIGHT];
    static const uint8_t classicDigit2[DIGIT_HEIGHT];
    static const uint8_t classicDigit3[DIGIT_HEIGHT];
    static const uint8_t classicDigit4[DIGIT_HEIGHT];
    static const uint8_t classicDigit5[DIGIT_HEIGHT];
    static const uint8_t classicDigit6[DIGIT_HEIGHT];
    static const uint8_t classicDigit7[DIGIT_HEIGHT];
    static const uint8_t classicDigit8[DIGIT_HEIGHT];
    static const uint8_t classicDigit9[DIGIT_HEIGHT];

    // HD44780-compatible digit bitmaps imported from a standard 5x7 LCD font.
    static const uint8_t hd44780Digit0[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit1[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit2[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit3[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit4[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit5[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit6[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit7[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit8[DIGIT_HEIGHT];
    static const uint8_t hd44780Digit9[DIGIT_HEIGHT];

    static FontStyle currentFont;
};

#endif // DIGITS5X8_H
