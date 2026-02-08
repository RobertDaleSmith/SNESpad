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

#include "snespad_c.h"

#ifdef ARDUINO
#include "Arduino.h"
#else
#include "pico/stdlib.h"
#endif

#if SNES_PAD_DEBUG
#include <stdio.h>
#endif

// ============================================================================
// Xband Keyboard Mapping Table
// ============================================================================
// Maps scancodes to key names and HID keycodes
// Indexed by [scancode & 0x0F][(scancode & 0xF0) >> 4]

static const snespad_key_mapping_t key_mapping[16][10] = {
    // 0xh, 1xh, 2xh, 3xh, 4xh, 5xh, 6xh, 7xh, 8xh, 9xh
    {
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"NUM-0", '0', 0},
        {"OpenX", KEY_LEFT_GUI, 1},
        {"90h", 0, 0}
    }, // x0h
    {
        {"F1", KEY_F1, 0},
        {"Alt", KEY_LEFT_ALT, 1},
        {"C", 'c', 0},
        {"N", 'n', 0},
        {",<", ',', 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"NUM-.", '.', 0},
        {"ClosedX", KEY_RIGHT_GUI, 1},
        {"91h", 0, 0}
    }, // x1h
    {
        {"F2", KEY_F2, 0},
        {"LShft", KEY_LEFT_SHIFT, 1},
        {"X", 'x', 0},
        {"B", 'b', 0},
        {"K", 'k', 0},
        {"\"'", '\'', 0},
        {"unused", 0, 0},
        {"NUM-2", '2', 0},
        {"unused", 0, 0},
        {"92h", 0, 0}
    }, // x2h
    {
        {"F3", KEY_F3, 0},
        {"unused", 0, 0},
        {"D", 'd', 0},
        {"H", 'h', 0},
        {"I", 'i', 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"NUM-5", '5', 0},
        {"unused", 0, 0},
        {"93h", 0, 0}
    }, // x3h
    {
        {"F4", KEY_F4, 0},
        {"LCtl", KEY_LEFT_CTRL, 1},
        {"E", 'e', 0},
        {"G", 'g', 0},
        {"O", 'o', 0},
        {"[{", '[', 0},
        {"unused", 0, 0},
        {"NUM-6", '6', 0},
        {"NUM-SUB", 0, 0},
        {"94h", 0, 0}
    }, // x4h
    {
        {"F5", KEY_F5, 0},
        {"Q", 'q', 0},
        {"4$", '4', 0},
        {"Y", 'y', 0},
        {"0)", '0', 0},
        {"=+", '=', 0},
        {"unused", 0, 0},
        {"NUM-8", '8', 0},
        {"unused", 0, 0},
        {"95h", 0, 0}
    }, // x5h
    {
        {"F6", KEY_F6, 0},
        {"1!", '1', 0},
        {"3#", '3', 0},
        {"6^", '6', 0},
        {"9(", '9', 0},
        {"unused", 0, 0},
        {"BACKSPACE", KEY_BACKSPACE, 0},
        {"ESC", KEY_ESC, 0},
        {"JOY-A", 0, 0},
        {"96h", 0, 0}
    }, // x6h
    {
        {"F7", KEY_F7, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"NUM-DIV", 0, 0},
        {"JOY-B", 0, 0},
        {"97h", 0, 0}
    }, // x7h
    {
        {"F8", KEY_F8, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"CAPS", KEY_CAPS_LOCK, 1},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"JOY-X", 0, 0},
        {"98h", 0, 0}
    }, // x8h
    {
        {"F9", KEY_F9, 0},
        {"unused", 0, 0},
        {"SPACE", ' ', 0},
        {"unused", 0, 0},
        {".>", '.', 0},
        {"RShft", KEY_RIGHT_SHIFT, 1},
        {"NUM-1", '1', 0},
        {"NUM-RET", 0, 0},
        {"JOY-Y", 0, 0},
        {"99h", 0, 0}
    }, // x9h
    {
        {"F10", KEY_F10, 0},
        {"Z", 'z', 0},
        {"V", 'v', 0},
        {"M", 'm', 0},
        {"/?", '/', 0},
        {"ENTER", KEY_RETURN, 0},
        {"unused", 0, 0},
        {"NUM-3", '3', 0},
        {"JOY-L", 0, 0},
        {"9Ah", 0, 0}
    }, // xAh
    {
        {"F11", KEY_F11, 0},
        {"S", 's', 0},
        {"F", 'f', 0},
        {"J", 'j', 0},
        {"L", 'l', 0},
        {"]}", ']', 0},
        {"NUM-4", '4', 0},
        {"unused", 0, 0},
        {"JOY-R", 0, 0},
        {"9Bh", 0, 0}
    }, // xBh
    {
        {"F12", KEY_F12, 0},
        {"A", 'a', 0},
        {"T", 't', 0},
        {"U", 'u', 0},
        {";:", ';', 0},
        {"unused", 0, 0},
        {"NUM-7", '7', 0},
        {"NUM-ADD", '+', 0},
        {"SELECT", 0, 0},
        {"9Ch", 0, 0}
    }, // xCh
    {
        {"Switch", KEY_TAB, 0},
        {"W", 'w', 0},
        {"R", 'r', 0},
        {"7&", '7', 0},
        {"P", 'p', 0},
        {"\\", '\\', 0},
        {"unused", 0, 0},
        {"NUM-9", '9', 0},
        {"START", 0, 0},
        {"9Dh", 0, 0}
    }, // xDh
    {
        {"`~", '`', 0},
        {"2@", '2', 0},
        {"5%", '5', 0},
        {"8*", '8', 0},
        {"-_", '-', 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"NUM-MUL", '*', 0},
        {"8Eh", 0, 0},
        {"unused", 0, 0}
    }, // xEh
    {
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"unused", 0, 0},
        {"8Fh", 0, 0},
        {"unused", 0, 0}
    }  // xFh
};

// ============================================================================
// Platform Abstraction
// ============================================================================

static inline void delay_us(unsigned int delay_value)
{
#ifdef ARDUINO
    delayMicroseconds(delay_value);
#else
    busy_wait_us(delay_value);
#endif
}

static inline void gpio_write(uint8_t pin, uint8_t value)
{
#ifdef ARDUINO
    digitalWrite(pin, value ? HIGH : LOW);
#else
    gpio_put(pin, value ? 1 : 0);
#endif
}

static inline uint8_t gpio_read(uint8_t pin)
{
#ifdef ARDUINO
    return digitalRead(pin);
#else
    return gpio_get(pin);
#endif
}

// ============================================================================
// Internal Functions
// ============================================================================

// Initialize GPIO pins
static void snespad_gpio_init(snespad_t* pad)
{
#ifdef ARDUINO
    pinMode(pad->clock_pin, OUTPUT);
    pinMode(pad->latch_pin, OUTPUT);
    pinMode(pad->data0_pin, INPUT);
    pinMode(pad->data1_pin, INPUT);
    pinMode(pad->iobit_pin, OUTPUT);

    digitalWrite(pad->data0_pin, HIGH);  // pull_up
    digitalWrite(pad->data1_pin, HIGH);
#else
    gpio_init(pad->clock_pin);
    gpio_init(pad->latch_pin);
    gpio_init(pad->data0_pin);
    gpio_init(pad->data1_pin);
    gpio_init(pad->iobit_pin);

    gpio_set_dir(pad->clock_pin, GPIO_OUT);
    gpio_set_dir(pad->latch_pin, GPIO_OUT);
    gpio_set_dir(pad->data0_pin, GPIO_IN);
    gpio_pull_up(pad->data0_pin);
    gpio_set_dir(pad->data1_pin, GPIO_IN);
    gpio_pull_up(pad->data1_pin);
    gpio_set_dir(pad->iobit_pin, GPIO_OUT);
#endif
}

// Signal mouse to change speed
static void snespad_set_mouse_speed(snespad_t* pad)
{
    if (pad->type == SNESPAD_MOUSE &&
        pad->mouse_speed_fails < SNES_MOUSE_THRESHOLD &&
        pad->mouse_speed != SNES_MOUSE_FAST) {

        gpio_write(pad->clock_pin, 0);
        delay_us(6);

        gpio_write(pad->clock_pin, 1);
        delay_us(12);
    }
}

// Clock in a single data bit (and shift out rumble data on IOBit)
static uint32_t snespad_clock_bit(snespad_t* pad, uint8_t data_pin)
{
    uint32_t ret;

    // Set IOBit for rumble data BEFORE clock pulse
    if (pad->rumble_active) {
        gpio_write(pad->iobit_pin, (pad->rumble_frame >> pad->rumble_bit_pos) & 1);
        if (pad->rumble_bit_pos == 0) {
            pad->rumble_bit_pos = 15;  // Wrap for continuous sending
        } else {
            pad->rumble_bit_pos--;
        }
    }

    gpio_write(pad->clock_pin, 0);
    delay_us(12);

    ret = gpio_read(data_pin);

    gpio_write(pad->clock_pin, 1);
    delay_us(12);

    return ret;
}

// Clock in data0/data1 dibits (for keyboard)
static uint32_t snespad_clock_dibit(snespad_t* pad)
{
    uint32_t ret;

    gpio_write(pad->clock_pin, 0);
    delay_us(12);

    ret = gpio_read(pad->data0_pin) | ((gpio_read(pad->data1_pin) & 1) << 1);

    gpio_write(pad->clock_pin, 1);
    delay_us(12);

    return ret;
}

// Latch to start read
static void snespad_latch(snespad_t* pad)
{
    gpio_write(pad->latch_pin, 1);
    delay_us(12);

    snespad_set_mouse_speed(pad);

    gpio_write(pad->latch_pin, 0);
    delay_us(12);
}

// Reverse bits within a byte (e.g., 0b1000 -> 0b0001)
static uint8_t snespad_reverse_byte(uint8_t c)
{
    uint8_t r = 0;
    for (int i = 0; i < 8; i++) {
        r <<= 1;
        r |= c & 1;
        c >>= 1;
    }
    return r;
}

// Read keyboard data
static bool snespad_read_keyboard(snespad_t* pad, bool readonly_id)
{
    uint8_t kid = 0;
    uint8_t num = 0;
    uint8_t i, n;

    // Activate the keyboard's host comm interrupt routine
    gpio_write(pad->iobit_pin, 0);

    // Read the 8 bits (4 clocks) keyboard signature (id)
    for (i = 0; i < 8; i += 2) {
        uint32_t bits = snespad_clock_dibit(pad);
        kid |= bits << i;

        if (i == 0 && readonly_id) {
            gpio_write(pad->iobit_pin, 1);  // KeyboardID read only (no scancodes)
        }

        // Auto recover common out of sync packet transaction
        if (i == 4 && kid == (SNES_KEYBOARD_ID >> 2)) {
            kid = kid << 2;
            break;
        }
    }

    // Can toggle caps lock after reading id
    if (!pad->caps_locked) {
        gpio_write(pad->iobit_pin, 1);  // Toggle Caps Lock LED
    }

    // Read the 4 bits (2 clocks) upcoming scancode byte count (0-15)
    for (i = 0; i < 4; i += 2) {
        uint32_t bits = snespad_clock_dibit(pad);
        num |= bits << i;
    }
    num = num & 0x0F;

    // Auto recover random bad ids
    if (pad->type == SNESPAD_KEYBOARD && kid == SNES_KEYBOARD_ID + 1) {
        kid = SNES_KEYBOARD_ID;
    }

    // Read the scancodes
    for (i = 0; i < 16; i++) {
        pad->scancodes[i] = 0;
    }

    if (!readonly_id && num && kid == SNES_KEYBOARD_ID) {
        for (n = 0; n < num; n++) {
            uint8_t read_byte = 0;

            for (i = 0; i < 8; i += 2) {
                uint32_t bits = snespad_clock_dibit(pad);
                read_byte |= bits << i;
            }

            pad->scancodes[n] = read_byte;
        }
    }

    // End keyboard read
    gpio_write(pad->iobit_pin, 1);
    pad->scancodes_len = num;

#if SNES_PAD_DEBUG
    printf("KB_ID:0x%02X SCANCODES: %d\n", kid, num);
#endif

    return kid == SNES_KEYBOARD_ID;
}

// Read device data
static uint32_t snespad_read(snespad_t* pad)
{
    uint32_t dat = 0;
    bool readonly_id = false;

    // A connected device will pull the data line low prior to latch
    // A disconnected pin is kept high by internal pull_up
    uint32_t disconnected = gpio_read(pad->data0_pin);

    // Normal 16-bit (or 32-bit for mouse) controller read
    snespad_latch(pad);
    for (uint8_t i = 0; i < 32; i++) {
        uint32_t bit = snespad_clock_bit(pad, pad->data0_pin);
        dat |= bit << i;

        if (i == 15) {
            bool read_extra = !bit;  // Check if mouse
            if (!read_extra) break;  // Skip extra bytes if not
            delay_us(12);
        }
    }

    // Check and read keyboard
    bool is_keyboard = snespad_read_keyboard(pad, readonly_id);

    dat = ~dat;  // Controller buttons are active low, so invert bits

    // Verify controller or mouse is connected
    if (!is_keyboard && disconnected && !(dat & 0xFFFF)) {
        pad->type = SNESPAD_NONE;
        pad->mouse_speed_fails = 0;
        pad->mouse_speed = 0;
        return 0;
    }

    // Check device type id
    bool is_snes = ((dat & SNES_DEVICE_ID) >> 12) == SNES_PAD_ID;
    bool is_mouse = ((dat & SNES_DEVICE_ID) >> 12) == SNES_MOUSE_ID;
    bool is_nes = ~dat && ((dat >> 8) & 0xFF) == 0xFF;

    // Determine device type
    if (is_keyboard) {
        pad->type = SNESPAD_KEYBOARD;
    } else if (is_snes) {
        pad->type = SNESPAD_CONTROLLER;
    } else if (is_nes) {
        pad->type = SNESPAD_NES;
    } else if (is_mouse) {
        uint8_t last_mouse_speed = pad->mouse_speed;

        // Parse mouse speed bits
        pad->mouse_speed = ((dat & SNES_MOUSE_SPEED) >> 10);
        if (pad->mouse_speed > 2) {
            pad->mouse_speed = 0;
        }

        // Detect Hyperkin mouse failure to change speed
        if (pad->mouse_speed != SNES_MOUSE_FAST &&
            last_mouse_speed == pad->mouse_speed &&
            pad->mouse_speed_fails < SNES_MOUSE_THRESHOLD) {
            pad->mouse_speed_fails++;
        }

        pad->type = SNESPAD_MOUSE;
    } else {
        pad->type = SNESPAD_NONE;

#if SNES_PAD_DEBUG
        printf("UNKNOWN device\n");
#endif
    }

    return dat;
}

// ============================================================================
// Public API Implementation
// ============================================================================

void snespad_init(snespad_t* pad, uint8_t clock, uint8_t latch,
                  uint8_t data0, uint8_t data1, uint8_t iobit)
{
    pad->latch_pin = latch;
    pad->clock_pin = clock;
    pad->data0_pin = data0;
    pad->data1_pin = data1;
    pad->iobit_pin = iobit;

    pad->type = SNESPAD_NONE;

    pad->button_a = false;
    pad->button_b = false;
    pad->button_x = false;
    pad->button_y = false;
    pad->button_start = false;
    pad->button_select = false;
    pad->button_l = false;
    pad->button_r = false;

    pad->direction_up = false;
    pad->direction_down = false;
    pad->direction_left = false;
    pad->direction_right = false;

    pad->mouse_x = 0;
    pad->mouse_y = 0;
    pad->mouse_speed = 0;
    pad->mouse_speed_fails = 0;

    pad->scancodes_len = 0;
    pad->caps_locked = false;
    pad->last_read = 0;

    pad->rumble_frame = 0;
    pad->rumble_bit_pos = 15;
    pad->rumble_active = false;
    pad->rumble_left = 0;
    pad->rumble_right = 0;

    for (int i = 0; i < 16; i++) {
        pad->scancodes[i] = 0;
    }
}

void snespad_begin(snespad_t* pad)
{
    snespad_gpio_init(pad);

#if SNES_PAD_DEBUG
    printf("snespad_begin\n");
#endif
}

void snespad_start(snespad_t* pad)
{
    uint32_t packet;

#if SNES_PAD_DEBUG
    printf("snespad_start\n");
#endif

    pad->type = SNESPAD_NONE;

    packet = snespad_read(pad);
    (void)packet;  // Packet used only for type detection in read()

#if SNES_PAD_DEBUG
    printf("Data Packet: 0x%08X\n", packet);
#endif

    if (pad->type != SNESPAD_NONE) {
#if SNES_PAD_DEBUG
        printf("Device Type: %d\n", pad->type);
#endif
        // Reset state
        pad->mouse_x = 0;
        pad->mouse_y = 0;

        pad->button_a = false;
        pad->button_b = false;
        pad->button_x = false;
        pad->button_y = false;
        pad->button_start = false;
        pad->button_select = false;
        pad->button_l = false;
        pad->button_r = false;

        pad->direction_up = false;
        pad->direction_down = false;
        pad->direction_left = false;
        pad->direction_right = false;
    }
#if SNES_PAD_DEBUG
    else {
        printf("Unknown Device.\n");
    }
#endif
}

void snespad_poll(snespad_t* pad)
{
    int32_t state = 0;

#if SNES_PAD_DEBUG
    printf("snespad_poll: ");
#endif

    if (pad->type != SNESPAD_NONE) {
        state = snespad_read(pad);

        if (state) {
            switch (pad->type) {
                case SNESPAD_CONTROLLER:
                    pad->direction_left =  (state & SNES_LEFT) != 0;
                    pad->direction_up =    (state & SNES_UP) != 0;
                    pad->direction_right = (state & SNES_RIGHT) != 0;
                    pad->direction_down =  (state & SNES_DOWN) != 0;

                    pad->button_select = (state & SNES_SELECT) != 0;
                    pad->button_start =  (state & SNES_START) != 0;
                    pad->button_b =      (state & SNES_B) != 0;
                    pad->button_y =      (state & SNES_Y) != 0;
                    pad->button_a =      (state & SNES_A) != 0;
                    pad->button_x =      (state & SNES_X) != 0;
                    pad->button_l =      (state & SNES_L) != 0;
                    pad->button_r =      (state & SNES_R) != 0;
                    break;

                case SNESPAD_NES:
                    pad->direction_left =  (state & SNES_LEFT) != 0;
                    pad->direction_up =    (state & SNES_UP) != 0;
                    pad->direction_right = (state & SNES_RIGHT) != 0;
                    pad->direction_down =  (state & SNES_DOWN) != 0;

                    pad->button_select = (state & SNES_SELECT) != 0;
                    pad->button_start =  (state & SNES_START) != 0;
                    pad->button_b =      (state & SNES_Y) != 0;  // NES B is at SNES Y position
                    pad->button_a =      (state & SNES_B) != 0;  // NES A is at SNES B position
                    break;

                case SNESPAD_MOUSE: {
                    int x = 127;  // Center position [0-255]
                    int y = 127;

                    // Mouse X axis
                    x = (state & SNES_MOUSE_X) >> 25;
                    x = snespad_reverse_byte(x) * SNES_MOUSE_PRECISION;
                    if (state & SNES_MOUSE_X_SIGN) {
                        x = 127 - x;
                    } else {
                        x = 127 + x;
                    }

                    // Mouse Y axis
                    y = (state & SNES_MOUSE_Y) >> 17;
                    y = snespad_reverse_byte(y) * SNES_MOUSE_PRECISION;
                    if (state & SNES_MOUSE_Y_SIGN) {
                        y = 127 - y;
                    } else {
                        y = 127 + y;
                    }

                    pad->mouse_x = x;
                    pad->mouse_y = y;
                    pad->button_b = (state & SNES_X) != 0;
                    pad->button_a = (state & SNES_A) != 0;
                    break;
                }

                case SNESPAD_KEYBOARD:
                    // Keyboard scancodes were already read in snespad_read()
                    break;

                default:
                    break;
            }

#if SNES_PAD_DEBUG
            if (pad->last_read != (uint32_t)state) {
                printf("A:%d B:%d X:%d Y:%d L:%d R:%d Sel:%d Start:%d ",
                       pad->button_a, pad->button_b, pad->button_x, pad->button_y,
                       pad->button_l, pad->button_r, pad->button_select, pad->button_start);
                printf("Mouse X:%d Y:%d", pad->mouse_x, pad->mouse_y);
            }
            pad->last_read = state;
#endif
        } else {
            // Device disconnected or invalid read
            pad->type = SNESPAD_NONE;
            snespad_start(pad);
        }
    } else {
        snespad_start(pad);
    }

#if SNES_PAD_DEBUG
    printf("\n");
#endif
}

snespad_key_mapping_t snespad_get_key_from_scancode(uint8_t scancode, bool special)
{
    snespad_key_mapping_t key = {"unused", 0, 0};

    if (special) {
        // Special scancodes (preceded by SNES_KEY_SPECIAL scancode)
        switch (scancode) {
            case SNES_KEY_RETURN:
                key.key_string = "JOY-A";
                key.hid_keycode = KEY_RETURN;
                key.releasable = true;
                break;
            case SNES_KEY_LEFT:
                key.key_string = "LEFT";
                key.hid_keycode = KEY_LEFT_ARROW;
                key.releasable = true;
                break;
            case SNES_KEY_DOWN:
                key.key_string = "DOWN";
                key.hid_keycode = KEY_DOWN_ARROW;
                key.releasable = true;
                break;
            case SNES_KEY_RIGHT:
                key.key_string = "RIGHT";
                key.hid_keycode = KEY_RIGHT_ARROW;
                key.releasable = true;
                break;
            case SNES_KEY_UP:
                key.key_string = "UP";
                key.hid_keycode = KEY_UP_ARROW;
                key.releasable = true;
                break;
        }
    } else {
        // Normal scancodes
        key = key_mapping[(scancode & 0x0F)][((scancode & 0xF0) >> 4)];
    }

    return key;
}

void snespad_set_caps_lock_led(snespad_t* pad, bool enabled)
{
    pad->caps_locked = enabled;
}

void snespad_set_rumble(snespad_t* pad, uint8_t left, uint8_t right)
{
    // Scale 0-255 → 0-15
    uint8_t l = left >> 4;
    uint8_t r = right >> 4;

    // Only update if changed
    if (l == pad->rumble_left && r == pad->rumble_right) return;

    pad->rumble_left = l;
    pad->rumble_right = r;

    // Build frame: 0x72 header + rumble byte (high nibble = right, low = left)
    pad->rumble_frame = 0x7200 | (r << 4) | l;
    pad->rumble_bit_pos = 15;
    pad->rumble_active = true;

    // If both motors off, send one final frame with zeros to clear
    // (rumble_active stays true so the zero-frame gets clocked out)
}
