// SNES-2-Serial
// =============
//
// SNESpad example demonstrating
// SNES to USB serial monitoring.
//

#include <SNESpad.h>

#undef SNES_PAD_DEBUG
#define SNES_PAD_DEBUG false

// Pins for SNES controller
#define CLOCK_PIN 5
#define LATCH_PIN 6
#define DATA0_PIN 7
#define DATA1_PIN 8
#define IOSEL_PIN 9

SNESpad * snes = new SNESpad(CLOCK_PIN, LATCH_PIN, DATA0_PIN, DATA1_PIN, IOSEL_PIN);

int last_type = -1;
bool key_release = false;
bool key_special = false;
bool key_caps_locked = false;
bool key_caps_pressed = false;

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
  if (snes->type == SNES_PAD_CONTROLLER) {
    Serial.print("[SNES Controller] {");
    Serial.print("D-pad: ");
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
    Serial.print("}\n");
  }

  // print SNES controller button state
  if (snes->type == SNES_PAD_NES) {
    Serial.print("[NES Controller] {");
    Serial.print("D-pad: ");
    Serial.print(snes->directionUp ? 1 : 0);
    Serial.print(snes->directionRight ? 1 : 0);
    Serial.print(snes->directionDown ? 1 : 0);
    Serial.print(snes->directionLeft ? 1 : 0);
    
    Serial.print("Buttons: ");
    Serial.print("B:" ); Serial.print(snes->buttonB ? 1 : 0);
    Serial.print(", A:" ); Serial.print(snes->buttonA ? 1 : 0);
    Serial.print(", Select:"); Serial.print(snes->buttonSelect ? 1 : 0);
    Serial.print(", Start:"); Serial.print(snes->buttonStart ? 1 : 0);
    Serial.print("}\n");
  }


  // if SNES mouse, then map mouse output
  if (snes->type == SNES_PAD_MOUSE) {
    // signed mouse movement range
    int mouseX = snes->mouseX-127;
    int mouseY = snes->mouseY-127;

    // print mouse movements
    Serial.print("[SNES Mouse] {");

    // print mouse buttons
    Serial.print("Buttons: ");
    Serial.print("Left:" ); Serial.print(snes->buttonB ? 1 : 0);
    Serial.print(", Right:" ); Serial.print(snes->buttonA ? 1 : 0);
    
    Serial.print(" Movement: x: ");
    Serial.print(mouseX);
    Serial.print(" y: ");
    Serial.print(mouseY);
    Serial.print("}\n");
  }

  // is SNES Xband keyboard, then map hid output
  if (snes->type == SNES_PAD_KEYBOARD) {
    uint8_t num = snes->scancodes_len;
    uint8_t n;

    if (num) { // has scancodes
      Serial.print("[SNES Keyboard] {");
      Serial.print("Scancode: ");
      for (n = 0; n < num; n++) {
        Serial.print("0x");
        Serial.print(snes->scancodes[n], HEX);
        if (num > 1 && n < num-1) Serial.print(",");
        Serial.print(" ");
      }
      Serial.print("| ");

      for (n = 0; n < num; n++) {
        uint8_t scancode = snes->scancodes[n];
        // normal key key_release scancode
        if (scancode == SNES_KEY_RELEASE) {
          key_release = true;
          Serial.print("RELEASE ");
        }
    
        // special key key_release scancode
        else if (scancode == SNES_KEY_SPECIAL) {
          key_special = true;
          Serial.print("SPECIAL ");
        }
    
        // normal key scancodes
        else {
          // Print the character corresponding to the scancode.
          XbandKeyMapping key = snes->getKeyFromScancode(scancode, key_special);
          Serial.print(key.key_string);

          // Toggle capsLock on press and only again once released and pressed
          if (scancode == SNES_KEY_CAPS) {
            if (key_release) {
              key_caps_pressed = false;
            } else if (!key_caps_pressed) {
              key_caps_pressed = true;
              key_caps_locked = !key_caps_locked;
              snes->setCapsLockLed(key_caps_locked);
              Serial.print(key_caps_locked ? ":ON": ":OFF");
            }
          }
  
          if (key.releasable) {
            if (key_release) Serial.print(":0");
            else Serial.print(":1");
          }

          key_release = false;
          key_special = false;
        }
      }
  
      Serial.print("}\n");
    }
    
  }

  if (snes->type == SNES_PAD_NONE && last_type != SNES_PAD_NONE) {
    Serial.print("No device detected..");
  }

  last_type = snes->type;
}
