// SNES-2-USB (arduino/HID)
// ================
//
// SNESpad example demonstrating SNES to
// Arduino based HID joystick/mouse USB devices.
//

#include <SNESpad.h>
#include <HID.h>
#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA_PIN 7

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA_PIN);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  8, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  // initialize hid mouse output
  Mouse.begin();

  // initialize hid joystick output
  Joystick.begin();
  Joystick.setXAxisRange(-1, 1);
  Joystick.setYAxisRange(-1, 1);
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // map SNES controller button state
  Joystick.setButton(0, snes->buttonB);
  Joystick.setButton(1, snes->buttonA);
  Joystick.setButton(2, snes->buttonY);
  Joystick.setButton(3, snes->buttonX);
  Joystick.setButton(4, snes->buttonL);
  Joystick.setButton(5, snes->buttonR);
  Joystick.setButton(6, snes->buttonSelect);
  Joystick.setButton(7, snes->buttonStart);

  // map SNES controller d-pad to x-axis
  if (snes->directionLeft) Joystick.setXAxis(-1);
  else if (snes->directionRight) Joystick.setXAxis(1);
  else Joystick.setXAxis(0);

  // map SNES controller d-pad to y-axis
  if (snes->directionUp) Joystick.setYAxis(-1);
  else if (snes->directionDown) Joystick.setYAxis(1);
  else Joystick.setYAxis(0);

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

  // send hid joystick values to device
  Joystick.sendState();
}
