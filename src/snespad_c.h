/*
  SNESpad - Arduino/Pico library for interfacing with SNES controllers

  github.com/RobertDaleSmith/SNESpad

  Version: 3.0 (2024) - C port for broader compatibility (Robert Dale Smith)
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
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SNESPAD_C_H
#define SNESPAD_C_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Debug flag (set to 1 to enable debug output)
#ifndef SNES_PAD_DEBUG
#define SNES_PAD_DEBUG 0
#endif

// Device types
#define SNESPAD_NONE       -1
#define SNESPAD_CONTROLLER  0
#define SNESPAD_NES         1
#define SNESPAD_MOUSE       2
#define SNESPAD_KEYBOARD    3

// Device identification
#define SNES_PAD_ID         0x00
#define SNES_MOUSE_ID       0x08
#define SNES_KEYBOARD_ID    0x78

// Button masks (active after bit inversion)
#define SNES_B              0x0001
#define SNES_Y              0x0002
#define SNES_SELECT         0x0004
#define SNES_START          0x0008
#define SNES_UP             0x0010
#define SNES_DOWN           0x0020
#define SNES_LEFT           0x0040
#define SNES_RIGHT          0x0080
#define SNES_A              0x0100
#define SNES_X              0x0200
#define SNES_L              0x0400
#define SNES_R              0x0800
#define SNES_DEVICE_ID      0xF000

// Mouse-specific masks
#define SNES_MOUSE_SPEED    0x0C00
#define SNES_MOUSE_Y_SIGN   0x00010000
#define SNES_MOUSE_Y        0x00FE0000
#define SNES_MOUSE_X_SIGN   0x01000000
#define SNES_MOUSE_X        0xFE000000

// Mouse speed settings
#define SNES_MOUSE_SLOW     0
#define SNES_MOUSE_MEDIUM   2
#define SNES_MOUSE_FAST     1

#define SNES_MOUSE_THRESHOLD 10   // max speed fails (Hyperkin compatibility)
#define SNES_MOUSE_PRECISION 1    // mouse movement velocity multiplier

// Keyboard scancodes
#define SNES_KEY_RELEASE    0xF0  // scancode: next key released
#define SNES_KEY_SPECIAL    0xE0  // scancode: next key special

// Special key scancodes (after SNES_KEY_SPECIAL)
#define SNES_KEY_RETURN     0x5A
#define SNES_KEY_LEFT       0x6B
#define SNES_KEY_DOWN       0x72
#define SNES_KEY_RIGHT      0x74
#define SNES_KEY_UP         0x75

// Arduino Keyboard.h style keycodes (for compatibility)
#ifndef KEY_LEFT_CTRL
#define KEY_LEFT_CTRL       0x80
#define KEY_LEFT_SHIFT      0x81
#define KEY_LEFT_ALT        0x82
#define KEY_LEFT_GUI        0x83
#define KEY_RIGHT_SHIFT     0x85
#define KEY_RIGHT_GUI       0x87
#define KEY_RETURN          0xB0
#define KEY_ESC             0xB1
#define KEY_BACKSPACE       0xB2
#define KEY_TAB             0xB3
#define KEY_CAPS_LOCK       0xC1
#define KEY_F1              0xC2
#define KEY_F2              0xC3
#define KEY_F3              0xC4
#define KEY_F4              0xC5
#define KEY_F5              0xC6
#define KEY_F6              0xC7
#define KEY_F7              0xC8
#define KEY_F8              0xC9
#define KEY_F9              0xCA
#define KEY_F10             0xCB
#define KEY_F11             0xCC
#define KEY_F12             0xCD
#define KEY_UP_ARROW        0xDA
#define KEY_DOWN_ARROW      0xD9
#define KEY_LEFT_ARROW      0xD8
#define KEY_RIGHT_ARROW     0xD7
#endif

// Xband keyboard key mapping
typedef struct {
    const char* key_string;     // String representation of the key
    uint8_t hid_keycode;        // HID keyboard keycode
    bool releasable;            // Keys that get a released scancode
} snespad_key_mapping_t;

// SNESpad state structure
typedef struct {
    // Device type (-1 = none, 0 = controller, 1 = NES, 2 = mouse, 3 = keyboard)
    int8_t type;

    // Pin configuration
    uint8_t latch_pin;
    uint8_t clock_pin;
    uint8_t data0_pin;
    uint8_t data1_pin;
    uint8_t iobit_pin;

    // Controller/NES button state
    bool button_a;
    bool button_b;
    bool button_x;
    bool button_y;
    bool button_start;
    bool button_select;
    bool button_l;
    bool button_r;
    bool direction_up;
    bool direction_down;
    bool direction_left;
    bool direction_right;

    // Mouse state
    uint16_t mouse_x;
    uint16_t mouse_y;
    uint8_t mouse_speed;
    uint8_t mouse_speed_fails;

    // Keyboard state
    uint8_t scancodes[16];
    uint8_t scancodes_len;
    bool caps_locked;

    // Rumble output (LRG protocol via IOBit)
    uint16_t rumble_frame;      // 16-bit frame to shift out (0x72XX)
    uint8_t  rumble_bit_pos;    // Current bit position (15..0)
    bool     rumble_active;     // Whether we have a frame to send
    uint8_t  rumble_left;       // Current left motor intensity (0-15)
    uint8_t  rumble_right;      // Current right motor intensity (0-15)

    // Debug/internal
    uint32_t last_read;
} snespad_t;

// ============================================================================
// Public API
// ============================================================================

// Initialize SNESpad structure with pin configuration
// Parameters:
//   pad    - Pointer to snespad_t structure to initialize
//   clock  - GPIO pin for clock signal (output)
//   latch  - GPIO pin for latch signal (output)
//   data0  - GPIO pin for data0 line (input, directly from controller)
//   data1  - GPIO pin for data1 line (input, directly from controller)
//   iobit  - GPIO pin for I/O bit (output, used for keyboard/mouse)
void snespad_init(snespad_t* pad, uint8_t clock, uint8_t latch,
                  uint8_t data0, uint8_t data1, uint8_t iobit);

// Initialize GPIO pins for communication
// Call this after snespad_init() to set up the hardware
void snespad_begin(snespad_t* pad);

// Detect and initialize connected device
// Call this to detect what type of device is connected
// Sets pad->type to the detected device type
void snespad_start(snespad_t* pad);

// Poll current device state
// Call this regularly in main loop to update button/axis state
// If device disconnects, will automatically call snespad_start()
void snespad_poll(snespad_t* pad);

// Get key mapping from keyboard scancode
// Parameters:
//   scancode - The scancode read from keyboard
//   special  - true if preceded by SNES_KEY_SPECIAL (0xE0)
// Returns: Key mapping structure with string name and HID keycode
snespad_key_mapping_t snespad_get_key_from_scancode(uint8_t scancode, bool special);

// Set caps lock LED state for keyboard
// Parameters:
//   pad     - Pointer to snespad_t structure
//   enabled - true to turn on caps lock LED
void snespad_set_caps_lock_led(snespad_t* pad, bool enabled);

// Set rumble motor intensities (LRG SNES Rumble protocol)
// Sends 0x72 magic header + motor data via IOBit during clock cycles.
// Safe to call on standard controllers (IOBit is ignored).
// Parameters:
//   pad   - Pointer to snespad_t structure
//   left  - Left motor intensity (0-255, scaled to 0-15)
//   right - Right motor intensity (0-255, scaled to 0-15)
void snespad_set_rumble(snespad_t* pad, uint8_t left, uint8_t right);

#ifdef __cplusplus
}
#endif

#endif // SNESPAD_C_H
