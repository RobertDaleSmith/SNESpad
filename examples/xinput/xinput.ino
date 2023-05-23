// SNES-2-USB (XInput)
// ===================
//
// SNESpad example demonstrating
// SNES to XInput USB controller.
//

#include <SNESpad.h>
#include <XInput.h> // https://github.com/dmadison/ArduinoXInput#compatible-boards

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA_PIN 7

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA_PIN);

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  // initialize X-input output
  XInput.setRange(JOY_LEFT,  0, 255);
  XInput.setAutoSend(false);  // Wait for all controls before sending
  XInput.begin();
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // map SNES controller button state
  XInput.setButton(BUTTON_A, snes->buttonB);
  XInput.setButton(BUTTON_B, snes->buttonA);
  XInput.setButton(BUTTON_X, snes->buttonY);
  XInput.setButton(BUTTON_Y, snes->buttonX);
  XInput.setButton(BUTTON_LB, snes->buttonL);
  XInput.setButton(BUTTON_RB, snes->buttonR);
  XInput.setButton(BUTTON_BACK, snes->buttonSelect);
  XInput.setButton(BUTTON_START, snes->buttonStart);
  XInput.setDpad(snes->directionUp, snes->directionDown, snes->directionLeft, snes->directionRight);

  if (snes->type == SNES_PAD_MOUSE) {
    // maps mouse movement to analog
    XInput.setJoystickX(JOY_LEFT, snes->mouseX, false);
    XInput.setJoystickY(JOY_LEFT, snes->mouseY, true);

    // maps mouse movement to d-pad
    // int mouseX = snes->mouseX-127;
    // int mouseY = snes->mouseY-127;
    // XInput.setDpad(mouseY < 0, mouseY > 0, mouseX < 0, mouseX > 0);
  }

  // send xinput values to device
  XInput.send();
}
