/*
  SNESpad - Arduino/Pico library for interfacing with SNES controllers

  github.com/RobertDaleSmith/SNESpad

  Version: 2.0 (2023) - Extended to Pico SDK (Robert Dale Smith)
                      - Mouse and NES controller support (Robert Dale Smith)
  Version: 1.3 (11/12/2010) - get rid of shortcut constructor - seems to be broken
  Version: 1.2 (05/25/2009) - put pin numbers in constructor (Pascal Hahn)
  Version: 1.1 (09/22/2008) - fixed compilation errors in arduino 0012 (Rob Duarte)
  Version: 1.0 (09/20/2007) - Created (Rob Duarte)
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITSNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.  
*/

#ifndef _SNESPAD_H_
#define _SNESPAD_H_
#endif
#include <inttypes.h>
// Try to include Arduino.h
#ifdef ARDUINO
    #include <Arduino.h>
#else
    // If we aren't compiling on Arduino, include the Pico SDK standard library
    #include "pico/stdlib.h"
#endif

#ifndef SNES_PAD_DEBUG
#define SNES_PAD_DEBUG      0
#endif

#define SNES_PAD_NONE      -1
#define SNES_PAD_CONTROLLER 0
#define SNES_PAD_NES        1
#define SNES_PAD_MOUSE      2
#define SNES_PAD_KEYBOARD   3

#define SNES_B              0x01       // 0b0000000000000000 0000000000000001
#define SNES_Y              0x02       // 0b0000000000000000 0000000000000010
#define SNES_SELECT         0x04       // 0b0000000000000000 0000000000000100
#define SNES_START          0x08       // 0b0000000000000000 0000000000001000
#define SNES_UP             0x10       // 0b0000000000000000 0000000000010000
#define SNES_DOWN           0x20       // 0b0000000000000000 0000000000100000
#define SNES_LEFT           0x40       // 0b0000000000000000 0000000001000000
#define SNES_RIGHT          0x80       // 0b0000000000000000 0000000010000000
#define SNES_A              0x100      // 0b0000000000000000 0000000100000000
#define SNES_X              0x200      // 0b0000000000000000 0000001000000000
#define SNES_L              0x400      // 0b0000000000000000 0000010000000000
#define SNES_R              0x800      // 0b0000000000000000 0000100000000000
#define SNES_DEVICE_ID      0xF000     // 0b0000000000000000 1111000000000000
#define SNES_MOUSE_SPEED    0xC00      // 0b0000000000000000 0000110000000000
#define SNES_MOUSE_Y_SIGN   0x10000    // 0b0000000000000001 0000000000000000
#define SNES_MOUSE_Y        0xFE0000   // 0b0000000011111110 0000000000000000
#define SNES_MOUSE_X_SIGN   0x1000000  // 0b0000000100000000 0000000000000000
#define SNES_MOUSE_X        0xFE000000 // 0b1111111000000000 0000000000000000

#define SNES_PAD_ID         0x00
#define SNES_MOUSE_ID       0x08
#define SNES_KEYBOARD_ID    0x78 // Xband Keyboard ID (within keyboardRead)

#define SNES_MOUSE_SLOW     0
#define SNES_MOUSE_MEDIUM   2
#define SNES_MOUSE_FAST     1

#define SNES_MOUSE_THRESHOLD 10  // max speed fails (Hyperkin compatiblity)
#define SNES_MOUSE_PRECISION 1   // mouse movement velocity multiplier

// SNES Xband Keyboard Scancodes
#define SNES_KEY_RELEASE    0xf0 // scancode: next key released
#define SNES_KEY_SPECIAL    0xe0 // scancode: next key special

// Normal Key Scancodes Table
// column 0xh
#define SNES_KEY_F1         0x01
#define SNES_KEY_F2         0x02
#define SNES_KEY_F3         0x03
#define SNES_KEY_F4         0x04
#define SNES_KEY_F5         0x05
#define SNES_KEY_F6         0x06
#define SNES_KEY_F7         0x07
#define SNES_KEY_F8         0x08
#define SNES_KEY_F9         0x09
#define SNES_KEY_F10        0x0a
#define SNES_KEY_F11        0x0b
#define SNES_KEY_F12        0x0c
#define SNES_KEY_SWITCH     0x0d // TAB
#define SNES_KEY_TILDE      0x0e
// column 1xh
#define SNES_KEY_ALT        0x11
#define SNES_KEY_LEFT_SHIFT 0x12
#define SNES_KEY_RIGHT_CTRL 0x13
#define SNES_KEY_LEFT_CTRL  0x14
#define SNES_KEY_Q          0x15
#define SNES_KEY_1          0x16
#define SNES_KEY_F13        0x17
#define SNES_KEY_F14        0x18
#define SNES_KEY_F15        0x19
#define SNES_KEY_Z          0x1a
#define SNES_KEY_S          0x1b
#define SNES_KEY_A          0x1c
#define SNES_KEY_W          0x1d
#define SNES_KEY_2          0x1e
#define SNES_KEY_F16        0x1f
// column 2xh
#define SNES_KEY_C          0x21
#define SNES_KEY_X          0x22
#define SNES_KEY_D          0x23
#define SNES_KEY_E          0x24
#define SNES_KEY_4          0x25
#define SNES_KEY_3          0x26
#define SNES_KEY_SPACE      0x29
#define SNES_KEY_V          0x2a
#define SNES_KEY_F          0x2b
#define SNES_KEY_T          0x2c
#define SNES_KEY_R          0x2d
#define SNES_KEY_5          0x2e
// column 3xh
#define SNES_KEY_N          0x31
#define SNES_KEY_B          0x32
#define SNES_KEY_H          0x33
#define SNES_KEY_G          0x34
#define SNES_KEY_Y          0x35
#define SNES_KEY_6          0x36
#define SNES_KEY_M          0x3a
#define SNES_KEY_J          0x3b
#define SNES_KEY_U          0x3c
#define SNES_KEY_7          0x3d
#define SNES_KEY_8          0x3e
// column 4xh
#define SNES_KEY_COMMA      0x41
#define SNES_KEY_K          0x42
#define SNES_KEY_I          0x43
#define SNES_KEY_O          0x44
#define SNES_KEY_0          0x45
#define SNES_KEY_9          0x46
#define SNES_KEY_PERIOD     0x49
#define SNES_KEY_SLASH      0x4a
#define SNES_KEY_L          0x4b
#define SNES_KEY_SEMICOLON  0x4c
#define SNES_KEY_P          0x4d
#define SNES_KEY_HYPHEN     0x4e
// column 5xh
#define SNES_KEY_QUOTE      0x52
#define SNES_KEY_OPEN_BRACKET 0x54
#define SNES_KEY_EQUAL      0x55
#define SNES_KEY_CAPS       0x58
#define SNES_KEY_RIGHT_SHIFT 0x59
#define SNES_KEY_ENTER      0x5a
#define SNES_KEY_CLOSED_BRACKET 0x5b
#define SNES_KEY_BACKSLASH  0x5d
// column 6xh
#define SNES_KEY_BACKSPACE  0x66
#define SNES_KEY_NUM_1      0x69
#define SNES_KEY_NUM_4      0x6B
#define SNES_KEY_NUM_7      0x6C
// column 7xh
#define SNES_KEY_NUM_0      0x70
#define SNES_KEY_NUM_PERIOD 0x71
#define SNES_KEY_NUM_2      0x72
#define SNES_KEY_NUM_5      0x73
#define SNES_KEY_NUM_6      0x74
#define SNES_KEY_NUM_8      0x75
#define SNES_KEY_CANCEL     0x76 // ESC
#define SNES_KEY_NUM_DIV    0x77
#define SNES_KEY_NUM_RETURN 0x79
#define SNES_KEY_NUM_3      0x7a
#define SNES_KEY_NUM_ADD    0x7c
#define SNES_KEY_NUM_9      0x7d
#define SNES_KEY_NUM_MUL    0x7e
// column 8xh
#define SNES_KEY_OPEN_X     0x80
#define SNES_KEY_F17        0x82
#define SNES_KEY_NUM_SUB    0x84
#define SNES_KEY_JOY_A      0x86
#define SNES_KEY_JOY_B      0x87
#define SNES_KEY_JOY_X      0x88
#define SNES_KEY_JOY_Y      0x89
#define SNES_KEY_JOY_L      0x8A
#define SNES_KEY_JOY_R      0x8B
#define SNES_KEY_JOY_SELECT 0x8C
#define SNES_KEY_JOY_START  0x8D

// Special Key Scancodes
#define SNES_KEY_RETURN     0x5a // JOY-A?
#define SNES_KEY_LEFT       0x6b
#define SNES_KEY_DOWN       0x72
#define SNES_KEY_RIGHT      0x74
#define SNES_KEY_UP         0x75

// Arduino Keyboard.h Lib Keycodes
#ifndef KEY_LEFT_CTRL
#define KEY_LEFT_CTRL       0x80
#define KEY_LEFT_SHIFT      0x81
#define KEY_LEFT_ALT        0x82
#define KEY_LEFT_GUI        0x83
#define KEY_RIGHT_SHIFT     0x85
#define KEY_RIGHT_GUI       0x87
#define KEY_RETURN          0xb0
#define KEY_ESC             0xb1
#define KEY_BACKSPACE       0xb2
#define KEY_TAB             0xb3
#define KEY_CAPS_LOCK       0xc1
#define KEY_F1              0xc2
#define KEY_F2              0xc3
#define KEY_F3              0xc4
#define KEY_F4              0xc5
#define KEY_F5              0xc6
#define KEY_F6              0xc7
#define KEY_F7              0xc8
#define KEY_F8              0xc9
#define KEY_F9              0xca
#define KEY_F10             0xcb
#define KEY_F11             0xcc
#define KEY_F12             0xcd

#define KEY_UP_ARROW        0xda
#define KEY_DOWN_ARROW      0xd9
#define KEY_LEFT_ARROW      0xd8
#define KEY_RIGHT_ARROW     0xd7
#endif

typedef struct {
    const char* key_string; // String representation of the key
    uint8_t hid_keycode; // HID keyboard keycode
    bool releasable; // Keys that get a released scancode
} XbandKeyMapping;

class SNESpad {
  protected:
	// uint8_t address;
  public:
    int8_t type = SNES_PAD_NONE; 

    uint16_t mouseX        = 0;
    uint16_t mouseY        = 0;

    bool buttonA         = false;
    bool buttonB         = false;
    bool buttonX         = false;
    bool buttonY         = false;
    bool buttonStart     = false;
    bool buttonSelect    = false;
    bool buttonL         = false;
    bool buttonR         = false;

    bool directionUp     = false;
    bool directionDown   = false;
    bool directionLeft   = false;
    bool directionRight  = false;

    uint8_t scancodes[16] = {0};
    uint8_t scancodes_len = 0;

    // Constructor 
    SNESpad(int clock, int latch, int data0, int data1, int iobita);

    // Methods
    void begin();
  	void start();
  	void poll();
    XbandKeyMapping getKeyFromScancode(uint8_t scancode, bool special);
    bool setCapsLockLed(bool enabled);
  private:
	
    uint8_t latchPin; // output: latch
    uint8_t clockPin; // output: clock
    uint8_t data0Pin;  // input:  data0
    uint8_t data1Pin;  // input:  data1
    uint8_t iobitPin;  // input:  select
    uint8_t mouseSpeed;   // mouse speed (0=SLOW|1=MEDIUM|2=FAST)
    uint8_t mouseSpeedFails = 0;
    uint32_t _lastRead;
    bool capsLocked = false; // xband keyboard capslock state

    XbandKeyMapping keyMapping[16][10] = {
        // 0xh, 1xh, 2xh, 3xh, 4xh, 5xh, 6xh, 7xh, 8xh, 9xh
        {
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"NUM-0", '0'},
          {"OpenX", KEY_LEFT_GUI, 1},
          {"90h", 0}
        }, // x0h
        {
          {"F1", KEY_F1},
          {"Alt", KEY_LEFT_ALT, 1},
          {"C", 'c'},
          {"N", 'n'},
          {",<", ','},
          {"unused", 0},
          {"unused", 0},
          {"NUM-.", '.'},
          {"ClosedX", KEY_RIGHT_GUI, 1},
          {"91h", 0}
        }, // x1h
        {
          {"F2", KEY_F2},
          {"LShft", KEY_LEFT_SHIFT, 1},
          {"X", 'x'},
          {"B", 'b'},
          {"K", 'k'},
          {"\",", '\''},
          {"unused", 0},
          {"NUM-2", '2'},
          {"unused", 0},
          {"92h", 0}
        }, // x2h
        {
          {"F3", KEY_F3},
          {"unused", 0},
          {"D", 'd'},
          {"H", 'h'},
          {"I", 'i'},
          {"unused", 0},
          {"unused", 0},
          {"NUM-5", '5'},
          {"unused", 0},
          {"93h", 0}
        }, // x3h
        {
          {"F4", KEY_F4},
          {"LCtl", KEY_LEFT_CTRL, 1},
          {"E", 'e'},
          {"G", 'g'},
          {"O", 'o'},
          {"[{", '['},
          {"unused", 0},
          {"NUM-6", '6'},
          {"NUM-SUB", 0},
          {"94h", 0}
        }, // x4h
        {
          {"F5", KEY_F5},
          {"Q", 'q'},
          {"4$", '4'},
          {"Y", 'y'},
          {"0)", '0'},
          {"=+", '='},
          {"unused", 0},
          {"NUM-8", '8'},
          {"unused", 0},
          {"95h", 0}
        }, // x5h
        {
          {"F6", KEY_F6},
          {"1!", '1'},
          {"3#", '3'},
          {"6^", '6'},
          {"9(", '9'},
          {"unused", 0},
          {"BACKSPACE", KEY_BACKSPACE},
          {"ESC", KEY_ESC},
          {"JOY-A", 0},
          {"96h", 0}
        }, // x6h
        {
          {"F7", KEY_F7},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"NUM-DIV", 0},
          {"JOY-B", 0},
          {"97h", 0}
        }, // x7h
        {
          {"F8", KEY_F8},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"CAPS", KEY_CAPS_LOCK, 1},
          {"unused", 0},
          {"unused", 0},
          {"JOY-X", 0},
          {"98h", 0}
        }, // x8h
        {
          {"F9", KEY_F9},
          {"unused", 0},
          {"SPACE", ' '},
          {"unused", 0},
          {".>", '.'},
          {"RShft", KEY_RIGHT_SHIFT, 1},
          {"NUM-1", '1'},
          {"NUM-RET", 0},
          {"JOY-Y", 0},
          {"99h", 0}
        }, // x9h
        {
          {"F10", KEY_F10},
          {"Z", 'z'},
          {"V", 'v'},
          {"M", 'm'},
          {"/?", '/'},
          {"ENTER", KEY_RETURN},
          {"unused", 0},
          {"NUM-3", '3'},
          {"JOY-L", 0},
          {"9Ah", 0}
        }, // xAh
        {
          {"F11", KEY_F11},
          {"S", 's'},
          {"F", 'f'},
          {"J", 'j'},
          {"L", 'l'},
          {"]}", ']'},
          {"NUM-4", '4'},
          {"unused", 0},
          {"JOY-R", 0},
          {"9Bh", 0}
        }, // xBh
        {
          {"F12", KEY_F12},
          {"A", 'a'},
          {"T", 't'},
          {"U", 'u'},
          {";:", ';'},
          {"unused", 0},
          {"NUM-7", '7'},
          {"NUM-ADD", '+'},
          {"SELECT", 0},
          {"9Ch", 0}
        }, // xCh
        {
          {"Switch", KEY_TAB},
          {"W", 'w'},
          {"R", 'r'},
          {"7&", '7'},
          {"P", 'p'},
          {"\\", '\\'},
          {"unused", 0},
          {"NUM-9", '9'},
          {"START", 0},
          {"9Dh", 0}
        }, // xDh
        {
          {"`~", '`'},
          {"2@", '2'},
          {"5%", '5'},
          {"8*", '8'},
          {"-_", '-'},
          {"unused", 0},
          {"unused", 0},
          {"NUM-MUL", '*'},
          {"8Eh", 0},
          {"unused", 0},
        }, // xEh
        {
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"unused", 0},
          {"8Fh", 0},
          {"unused", 0},
        }, // xFh
    };

    void init();
    void latch();
    uint32_t read();
    uint32_t clock(uint8_t dat0Pin, uint8_t dat1Pin);
    uint32_t clock(uint8_t datPin);
    void setMouseSpeed(); // check and set mouse speed
    bool readKeyboard(bool readonlyID);
    uint8_t reverse(uint8_t c);
    void handleScancodes();
};
