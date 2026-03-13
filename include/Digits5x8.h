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
    /**
     * @brief Get bitmap data for a digit
     * @param digit Digit 0-9
     * @return Pointer to 5-byte array containing bitmap data
     */
    static const uint8_t* getDigit(uint8_t digit);

private:
    // Digit bitmaps - 5x8 pixels each
    static const uint8_t digit0[DIGIT_HEIGHT];
    static const uint8_t digit1[DIGIT_HEIGHT];
    static const uint8_t digit2[DIGIT_HEIGHT];
    static const uint8_t digit3[DIGIT_HEIGHT];
    static const uint8_t digit4[DIGIT_HEIGHT];
    static const uint8_t digit5[DIGIT_HEIGHT];
    static const uint8_t digit6[DIGIT_HEIGHT];
    static const uint8_t digit7[DIGIT_HEIGHT];
    static const uint8_t digit8[DIGIT_HEIGHT];
    static const uint8_t digit9[DIGIT_HEIGHT];
};

#endif // DIGITS5X8_H
