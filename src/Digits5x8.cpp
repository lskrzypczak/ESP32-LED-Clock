#include "Digits5x8.h"

Digits5x8::FontStyle Digits5x8::currentFont = Digits5x8::FontStyle::Classic;

// Digit 0: Rectangle shape
const uint8_t Digits5x8::classicDigit0[DIGIT_HEIGHT] = {
    0b01110,  // Row 0: .###.
    0b10001,  // Row 1: #...#
    0b10001,  // Row 2: #...#
    0b10001,  // Row 3: #...#
    0b10001,  // Row 4: #...#
    0b10001,  // Row 5: #...#
    0b01110,  // Row 6: .###.
    0b00000   // Row 7: .....
};

// Digit 1: Vertical line
const uint8_t Digits5x8::classicDigit1[DIGIT_HEIGHT] = {
    0b00100,  // Row 0:  ..#..
    0b00110,  // Row 1:  ..##.
    0b00100,  // Row 2:  ..#..
    0b00100,  // Row 3:  ..#..
    0b00100,  // Row 4:  ..#..
    0b00100,  // Row 5:  ..#..
    0b01110,  // Row 6:  .###.
    0b00000   // Row 7:  .....
};

// Digit 2: Top horizontal, middle horizontal, bottom horizontal
const uint8_t Digits5x8::classicDigit2[DIGIT_HEIGHT] = {
    0b01110,  // Row 0: .###.
    0b10001,  // Row 1: #...#
    0b10000,  // Row 2: #....
    0b01100,  // Row 3: .##..
    0b00010,  // Row 4: ...#.
    0b00001,  // Row 5: ....#
    0b11111,  // Row 6: #####
    0b00000   // Row 7: .....
};

// Digit 3: Two left-side curves
const uint8_t Digits5x8::classicDigit3[DIGIT_HEIGHT] = {
    0b01110,  // Row 0: .###.
    0b10001,  // Row 1: #...#
    0b10000,  // Row 2: #....
    0b01100,  // Row 3: .##..
    0b10000,  // Row 4: #....
    0b10001,  // Row 5: #...#
    0b01110,  // Row 6: .###.
    0b00000   // Row 7: .....
};

// Digit 4: Left side with middle horizontal
const uint8_t Digits5x8::classicDigit4[DIGIT_HEIGHT] = {
    0b10001,  // Row 0: #...#
    0b10001,  // Row 1: #...#
    0b10001,  // Row 2: #...#
    0b11111,  // Row 3: #####
    0b10000,  // Row 4: #....
    0b10000,  // Row 5: #....
    0b10000,  // Row 6: #....
    0b00000   // Row 7: .....
};

// Digit 5: Top horizontal, middle, bottom left
const uint8_t Digits5x8::classicDigit5[DIGIT_HEIGHT] = {
    0b11111,  // Row 0: #####
    0b00001,  // Row 1: ....#
    0b01111,  // Row 2: .####
    0b10000,  // Row 3: #....
    0b10000,  // Row 4: #....
    0b10001,  // Row 5: #...#
    0b01110,  // Row 6: .###.
    0b00000   // Row 7: .....
};

// Digit 6: Right-side curve, middle horizontal, bottom-left
const uint8_t Digits5x8::classicDigit6[DIGIT_HEIGHT] = {
    0b01100,  // Row 0: .##..
    0b00010,  // Row 1: ...#.
    0b00001,  // Row 2: ....#
    0b01111,  // Row 3: .####
    0b10001,  // Row 4: #...#
    0b10001,  // Row 5: #...#
    0b01110,  // Row 6: .###.
    0b00000   // Row 7: .....
};

// Digit 7: Top horizontal, diagonal to bottom-left
const uint8_t Digits5x8::classicDigit7[DIGIT_HEIGHT] = {
    0b11111,  // Row 0: #####
    0b10000,  // Row 1: #....
    0b01000,  // Row 2: .#...
    0b00100,  // Row 3: ..#..
    0b00010,  // Row 4: ...#.
    0b00010,  // Row 5: ...#.
    0b00001,  // Row 6: ....#
    0b00000   // Row 7: .....
};

// Digit 8: Full rectangle (two stacked circles)
const uint8_t Digits5x8::classicDigit8[DIGIT_HEIGHT] = {
    0b01110,  // Row 0: .###.
    0b10001,  // Row 1: #...#
    0b10001,  // Row 2: #...#
    0b01110,  // Row 3: .###.
    0b10001,  // Row 4: #...#
    0b10001,  // Row 5: #...#
    0b01110,  // Row 6: .###.
    0b00000   // Row 7: .....
};

// Digit 9: Top curve, middle horizontal, left-side tail
const uint8_t Digits5x8::classicDigit9[DIGIT_HEIGHT] = {
    0b01110,  // Row 0: .###.
    0b10001,  // Row 1: #...#
    0b10001,  // Row 2: #...#
    0b11110,  // Row 3: ####.
    0b10000,  // Row 4: #....
    0b01000,  // Row 5: .#...
    0b00110,  // Row 6: ..##.
    0b00000   // Row 7: .....
};

// HD44780-compatible digits imported from the standard 5x7 LCD font table
// and expanded to this project's 5x8 row format with a blank bottom row.
const uint8_t Digits5x8::hd44780Digit0[DIGIT_HEIGHT] = {
    0b01110,
    0b10001,
    0b11001,
    0b10101,
    0b10011,
    0b10001,
    0b01110,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit1[DIGIT_HEIGHT] = {
    0b00100,
    0b00110,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b01110,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit2[DIGIT_HEIGHT] = {
    0b01110,
    0b10001,
    0b10000,
    0b01110,
    0b00001,
    0b00001,
    0b11111,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit3[DIGIT_HEIGHT] = {
    0b11111,
    0b10000,
    0b01000,
    0b01100,
    0b10000,
    0b10001,
    0b01110,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit4[DIGIT_HEIGHT] = {
    0b01000,
    0b01100,
    0b01010,
    0b01001,
    0b11111,
    0b01000,
    0b01000,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit5[DIGIT_HEIGHT] = {
    0b11111,
    0b00001,
    0b01111,
    0b10000,
    0b10000,
    0b10001,
    0b01110,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit6[DIGIT_HEIGHT] = {
    0b11100,
    0b00010,
    0b00001,
    0b01111,
    0b10001,
    0b10001,
    0b01110,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit7[DIGIT_HEIGHT] = {
    0b11111,
    0b10000,
    0b10000,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit8[DIGIT_HEIGHT] = {
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0b00000
};

const uint8_t Digits5x8::hd44780Digit9[DIGIT_HEIGHT] = {
    0b01110,
    0b10001,
    0b10001,
    0b11110,
    0b10000,
    0b01000,
    0b00111,
    0b00000
};

const uint8_t* Digits5x8::getDigit(uint8_t digit) {
    return getDigit(digit, currentFont);
}

const uint8_t* Digits5x8::getDigit(uint8_t digit, FontStyle font) {
    switch (digit) {
        case 0: return font == FontStyle::HD44780 ? hd44780Digit0 : classicDigit0;
        case 1: return font == FontStyle::HD44780 ? hd44780Digit1 : classicDigit1;
        case 2: return font == FontStyle::HD44780 ? hd44780Digit2 : classicDigit2;
        case 3: return font == FontStyle::HD44780 ? hd44780Digit3 : classicDigit3;
        case 4: return font == FontStyle::HD44780 ? hd44780Digit4 : classicDigit4;
        case 5: return font == FontStyle::HD44780 ? hd44780Digit5 : classicDigit5;
        case 6: return font == FontStyle::HD44780 ? hd44780Digit6 : classicDigit6;
        case 7: return font == FontStyle::HD44780 ? hd44780Digit7 : classicDigit7;
        case 8: return font == FontStyle::HD44780 ? hd44780Digit8 : classicDigit8;
        case 9: return font == FontStyle::HD44780 ? hd44780Digit9 : classicDigit9;
        default: return font == FontStyle::HD44780 ? hd44780Digit0 : classicDigit0;
    }
}

void Digits5x8::setFont(FontStyle font) {
    currentFont = font;
}

Digits5x8::FontStyle Digits5x8::getFont() {
    return currentFont;
}
