// SNES-2-Serial
// =============
//
// SNESpad example demonstrating
// SNES to USB serial monitoring.
//

#include <SNESpad.h>

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA_PIN 7

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA_PIN);

void setup() {
  // initialize snes controller reading
  snes->begin(); // init snes gpio
  snes->start(); // init snes read

  Serial.begin(115200);
  Serial.print("SNESpad\n");
}

void loop() {
  // read SNES controller button state
  snes->poll();

  // print SNES controller button state
  if (snes->type == SNES_PAD_BASIC) {
    Serial.print("[SNES Controller] {");
    Serial.print(" D-pad: ");
    Serial.print(snes->directionUp ? 1 : 0);
    Serial.print(snes->directionRight ? 1 : 0);
    Serial.print(snes->directionDown ? 1 : 0);
    Serial.print(snes->directionLeft ? 1 : 0);
    
    Serial.print(" Buttons: ");
    Serial.print("B:" ); Serial.print(snes->buttonB ? 1 : 0);
    Serial.print(", A:" ); Serial.print(snes->buttonA ? 1 : 0);
    Serial.print(", Y:" ); Serial.print(snes->buttonY ? 1 : 0);
    Serial.print(", X:" ); Serial.print(snes->buttonX ? 1 : 0);
    Serial.print(", L:" ); Serial.print(snes->buttonL ? 1 : 0);
    Serial.print(", R:" ); Serial.print(snes->buttonR ? 1 : 0);
    Serial.print(", Select:"); Serial.print(snes->buttonSelect ? 1 : 0);
    Serial.print(", Start:"); Serial.print(snes->buttonStart ? 1 : 0);
  }

  // print SNES controller button state
  if (snes->type == SNES_PAD_NES) {
    Serial.print("[NES Controller] {");
    Serial.print(" D-pad: ");
    Serial.print(snes->directionUp ? 1 : 0);
    Serial.print(snes->directionRight ? 1 : 0);
    Serial.print(snes->directionDown ? 1 : 0);
    Serial.print(snes->directionLeft ? 1 : 0);
    
    Serial.print(" Buttons: ");
    Serial.print("B:" ); Serial.print(snes->buttonB ? 1 : 0);
    Serial.print(", A:" ); Serial.print(snes->buttonA ? 1 : 0);
    Serial.print(", Select:"); Serial.print(snes->buttonSelect ? 1 : 0);
    Serial.print(", Start:"); Serial.print(snes->buttonStart ? 1 : 0);
  }


  // if SNES mouse, then map mouse output
  if (snes->type == SNES_PAD_MOUSE) {
    // signed mouse movement range
    int mouseX = snes->mouseX-127;
    int mouseY = snes->mouseY-127;

    // print mouse movements
    Serial.print("[SNES Mouse] {");

    // print mouse buttons
    Serial.print(" Buttons: ");
    Serial.print("Left:" ); Serial.print(snes->buttonB ? 1 : 0);
    Serial.print(", Right:" ); Serial.print(snes->buttonA ? 1 : 0);
    
    Serial.print(" Movement: x: ");
    Serial.print(mouseX);
    Serial.print(" y: ");
    Serial.print(mouseY);
  }

  if (snes->type == SNES_PAD_NONE) {
    Serial.print("No device detected..");
  }
  Serial.print("}\n");
}
