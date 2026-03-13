#include "Digits5x8.h"

// Digit 0: Rectangle shape
const uint8_t Digits5x8::digit0[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit1[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit2[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit3[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit4[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit5[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit6[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit7[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit8[DIGIT_HEIGHT] = {
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
const uint8_t Digits5x8::digit9[DIGIT_HEIGHT] = {
    0b01110,  // Row 0: .###.
    0b10001,  // Row 1: #...#
    0b10001,  // Row 2: #...#
    0b11110,  // Row 3: ####.
    0b10000,  // Row 4: #....
    0b01000,  // Row 5: .#...
    0b00110,  // Row 6: ..##.
    0b00000   // Row 7: .....
};

const uint8_t* Digits5x8::getDigit(uint8_t digit) {
    switch (digit) {
        case 0: return digit0;
        case 1: return digit1;
        case 2: return digit2;
        case 3: return digit3;
        case 4: return digit4;
        case 5: return digit5;
        case 6: return digit6;
        case 7: return digit7;
        case 8: return digit8;
        case 9: return digit9;
        default: return digit0;  // Default to 0 for invalid input
    }
}
