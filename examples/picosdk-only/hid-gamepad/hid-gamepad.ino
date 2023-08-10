// SNES-2-USB (pico/HID-joystick)
// ================
//
// SNESpad example demonstrating SNES to
// RP2040 based HID joystick USB devices.
//

#include <SNESpad.h>
#include <Joystick.h> // https://github.com/benjaminaigner/Joystick/blob/main/docs/api.md

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA0_PIN 7
#define DATA1_PIN 8
#define IOSEL_PIN 9

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA0_PIN, DATA1_PIN, IOSEL_PIN);

int getJoystickHatAngle(bool up, bool down, bool left, bool right) {
    if (up) {
        if (right) return 45;
        if (left) return 315;
        return 0;
    }
    if (down) {
        if (right) return 135;
        if (left) return 225;
        return 180;
    }
    if (right) return 90;
    if (left) return 270;

    // If none are pressed or conflicting inputs are given
    return -1;
}

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  // initialize hid joystick output
  Joystick.begin();
  Joystick.use8bit(true);
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // map SNES controller button state
  Joystick.button(1, snes->buttonY); // BTTN WEST
  Joystick.button(2, snes->buttonB); // BTTN SOUTH
  Joystick.button(3, snes->buttonA); // BTTN EAST
  Joystick.button(4, snes->buttonX); // BTTN NORTH
  Joystick.button(5, snes->buttonL); // L
  Joystick.button(6, snes->buttonR); // R
  Joystick.button(7, false);  // L2
  Joystick.button(8, false);  // R2
  Joystick.button(9, snes->buttonSelect); // Select
  Joystick.button(10, snes->buttonStart); // Start
//  Joystick.button(11, false); // L3
//  Joystick.button(12, false); // R3
//  Joystick.button(17, false); // PS/XBOX Button
//  Joystick.button(18, false); // PS Touch Pad
//  Joystick.button(13, snes->directionUp);    // DPAD UP
//  Joystick.button(14, snes->directionDown);  // DPAD DOWN
//  Joystick.button(15, snes->directionLeft);  // DPAD LEFT
//  Joystick.button(16, snes->directionRight); // DPAD RIGHT

  Joystick.hat(getJoystickHatAngle(snes->directionUp, snes->directionDown, snes->directionLeft, snes->directionRight));

  // // DPAD to Analog Stick
   
  // if (btns & SNES_LEFT) Joystick.X(-127);
  // else if (btns & SNES_RIGHT) Joystick.X(128);
  // else Joystick.X(0);
  //
  // if (btns & SNES_DOWN) Joystick.Y(-127);
  // else if (btns & SNES_UP) Joystick.Y(128);
  // else Joystick.Y(0);

  // if SNES mouse, then map mouse output
  if (snes->type == SNES_PAD_MOUSE) {
    int mouseX = snes->mouseX-127;  //set center position [0-255]
    int mouseY = snes->mouseY-127;

    // mouse to left analog stick
    Joystick.X(mouseX);
    Joystick.Y(mouseY);

    // mouse to dpad buttons
    //Joystick.button(13, mouseY > 0); // North
    //Joystick.button(14, mouseY < 0); // South
    //Joystick.button(15, mouseX > 0); // East
    //Joystick.button(16, mouseX < 0); // West
  }
}
