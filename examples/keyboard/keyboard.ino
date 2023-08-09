// SNES-2-Keyboard
// =============
//
// SNESpad example demonstrating
// SNES keyboard to USB HID keyboard.
//

#include <SNESpad.h>
#include <Keyboard.h>

#undef SNES_PAD_DEBUG
#define SNES_PAD_DEBUG false

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA0_PIN 7
#define DATA1_PIN 8
#define IOSEL_PIN 9

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA0_PIN, DATA1_PIN, IOSEL_PIN);

bool key_release = false;
bool key_special = false;
bool key_caps_locked = false;
bool key_caps_pressed = false;

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  // initialize HID keyboard output
  Keyboard.begin();
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // print SNES controller button state
  if (snes->type == SNES_PAD_CONTROLLER) {
    if (snes->directionUp) Keyboard.press(KEY_UP_ARROW);
    else Keyboard.release(KEY_UP_ARROW);
    if (snes->directionRight) Keyboard.press(KEY_RIGHT_ARROW);
    else Keyboard.release(KEY_RIGHT_ARROW);
    if (snes->directionDown) Keyboard.press(KEY_DOWN_ARROW);
    else Keyboard.release(KEY_DOWN_ARROW);
    if (snes->directionLeft) Keyboard.press(KEY_LEFT_ARROW);
    else Keyboard.release(KEY_LEFT_ARROW);

    if (snes->buttonB) Keyboard.press('b');
    else Keyboard.release('b');
    if (snes->buttonA) Keyboard.press('a');
    else Keyboard.release('a');
    if (snes->buttonY) Keyboard.press('y');
    else Keyboard.release('y');
    if (snes->buttonX) Keyboard.press('x');
    else Keyboard.release('x');
    if (snes->buttonL) Keyboard.press('l');
    else Keyboard.release('l');
    if (snes->buttonR) Keyboard.press('r');
    else Keyboard.release('r');

    if (snes->buttonSelect) Keyboard.press(KEY_ESC);
    else Keyboard.release(KEY_ESC);
    if (snes->buttonStart) Keyboard.press(KEY_RETURN);
    else Keyboard.release(KEY_RETURN);
  }

  // is SNES Xband keyboard, then map hid output
  if (snes->type == SNES_PAD_KEYBOARD) {
    uint8_t num = snes->scancodes_len;
    uint8_t n;

    if (num) { // has scancodes
      for (n = 0; n < num; n++) {
        uint8_t scancode = snes->scancodes[n];
        // normal key key_release scancode
        if (scancode == SNES_KEY_RELEASE) {
          key_release = true;
        }
    
        // special key key_release scancode
        else if (scancode == SNES_KEY_SPECIAL) {
          key_special = true;
        }
    
        // normal key scancodes
        else {
          // Print the character corresponding to the scancode.
          XbandKeyMapping key = snes->getKeyFromScancode(scancode, key_special);

          // Toggle capsLock on press and only again once released and pressed
          if (scancode == SNES_KEY_CAPS) {
            if (key_release) {
              key_caps_pressed = false;
            } else if (!key_caps_pressed) {
              key_caps_pressed = true;
              key_caps_locked = !key_caps_locked;
              snes->setCapsLockLed(key_caps_locked);
            }
          }

          // handle keyboard output
          if (key_release) {
             Keyboard.release(key.hid_keycode);
          } else if (key.releasable) {
             Keyboard.press(key.hid_keycode);
          } else {
             Keyboard.write(key.hid_keycode);
          }

          key_release = false;
          key_special = false;
        }
      }
    }
  }
}
