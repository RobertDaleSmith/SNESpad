// SNES-2-USB (HID-mouse)
// ================
//
// SNESpad example demonstrating
// SNES mouse to USB HID mouse.
//

#include <SNESpad.h>
#include <Mouse.h>

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA0_PIN 7
#define DATA1_PIN 8
#define IOSEL_PIN 9

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA0_PIN, DATA1_PIN, IOSEL_PIN);

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  // initialize hid mouse output
  Mouse.begin();
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // if SNES mouse, then map mouse output
  if (snes->type == SNES_PAD_MOUSE) {
    int mouseX = snes->mouseX-127;  //set center position [0-255]
    int mouseY = snes->mouseY-127;

    // send hid mouse movements
    Mouse.move(mouseX, mouseY);

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
}
