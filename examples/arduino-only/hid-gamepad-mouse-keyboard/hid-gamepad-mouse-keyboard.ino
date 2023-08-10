// SNES-2-USB (arduino/hid-gamepad-mouse-keyboard)
// ================
//
// SNESpad example demonstrating SNES to Arduino based
// HID gamepad/keyboard/mouse USB all-in-one devices.
//

#include <SNESpad.h>
#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/
#include <Keyboard.h>

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA0_PIN 7
#define DATA1_PIN 8
#define IOSEL_PIN 9

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA0_PIN, DATA1_PIN, IOSEL_PIN);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  10, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

bool key_release = false;
bool key_special = false;
bool key_caps_locked = false;
bool key_caps_pressed = false;

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  // initialize hid joystick output
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);

  // initialize hid mouse output
  Mouse.begin();

  // initialize hid keyboard output
  Keyboard.begin();
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // if SNES mouse, then map mouse output
  if (snes->type == SNES_PAD_CONTROLLER || snes->type == SNES_PAD_NES) {
    // map SNES controller button state
    Joystick.setButton(0, snes->buttonY);
    Joystick.setButton(1, snes->buttonB);
    Joystick.setButton(2, snes->buttonA);
    Joystick.setButton(3, snes->buttonX);
    Joystick.setButton(4, snes->buttonL);
    Joystick.setButton(5, snes->buttonR);
    Joystick.setButton(6, 0); // unused
    Joystick.setButton(7, 0); // unused
    Joystick.setButton(8, snes->buttonSelect);
    Joystick.setButton(9, snes->buttonStart);

    // map SNES controller d-pad to x-axis
    if (snes->directionLeft) Joystick.setXAxis(-1);
    else if (snes->directionRight) Joystick.setXAxis(1);
    else Joystick.setXAxis(0);

    // map SNES controller d-pad to y-axis
    if (snes->directionUp) Joystick.setYAxis(-1);
    else if (snes->directionDown) Joystick.setYAxis(1);
    else Joystick.setYAxis(0);

    // send hid joystick values to device
    Joystick.sendState();
  }

  // if SNES mouse, then map mouse output
  if (snes->type == SNES_PAD_MOUSE) {
    int x = snes->mouseX;  //set center position [0-255]
    int y = snes->mouseY;

    // hid mouse movement
    int mouseX = snes->mouseX-127;
    int mouseY = snes->mouseY-127;

    // send hid mouse movements
    Mouse.move(mouseX, mouseY);

    // send hid joystick movements
    if (mouseY > 0) Joystick.setYAxis(1);  // North
    if (mouseY < 0) Joystick.setYAxis(-1); // South
    if (mouseX > 0) Joystick.setXAxis(1);  // East
    if (mouseX < 0) Joystick.setXAxis(-1); // West

    // hid mouse left click
    if (snes->buttonB) {
      // if the mouse is not pressed, press it:
      if (!Mouse.isPressed(MOUSE_LEFT)) Mouse.press(MOUSE_LEFT);
    } else {
      // if the mouse is pressed, release it:
      if (Mouse.isPressed(MOUSE_LEFT)) Mouse.release(MOUSE_LEFT);
    }

    // hid mouse right click
    if (snes->buttonA) {
      // if the mouse is not pressed, press it:
      if (!Mouse.isPressed(MOUSE_RIGHT)) Mouse.press(MOUSE_RIGHT);
    } else {
      // if the mouse is pressed, release it:
      if (Mouse.isPressed(MOUSE_RIGHT)) Mouse.release(MOUSE_RIGHT);
    }
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
