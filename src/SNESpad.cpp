/*
  SNESpad - Arduino/Pico library for interfacing with SNES controllers

  github.com/RobertDaleSmith/SNESpad

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
  MERCHANTABILITY or FITSNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "SNESpad.h"

#ifdef ARDUINO
#include "Arduino.h"
#endif

SNESpad::SNESpad(int clock, int latch, int data0, int data1, int select) {
  latchPin = latch;
  clockPin = clock;
  data0Pin = data0;
  data1Pin = data1;
  iobitPin = select;
}

void custom_delay_us(unsigned int delay_value) {
#ifdef ARDUINO
  delayMicroseconds(delay_value);
#else
  busy_wait_us(delay_value);
#endif
}

void custom_write(uint8_t outPin, uint8_t dir) {
#ifdef ARDUINO
    digitalWrite(outPin, dir ? HIGH:LOW);
#else
    gpio_put(outPin, dir ? 1:0);
#endif
}

uint8_t custom_read(uint8_t inPin) {
#ifdef ARDUINO
    return digitalRead(inPin);
#else
    return gpio_get(inPin);
#endif
}

void SNESpad::begin() {
  init();
#if SNES_PAD_DEBUG==true
  Serial.print("SNESpad::begin\n");
#endif
}

void SNESpad::start() {
  uint32_t packet;

#if SNES_PAD_DEBUG==true
  Serial.print("SNESpad::start\n");
#endif

  type = SNES_PAD_NONE;

  packet = read();

#if SNES_PAD_DEBUG==true
  Serial.print("Data Packet: ");
  Serial.print(packet. BIN);
  Serial.print((packet & 0xff00) >> 8, BIN);
  Serial.print("\n");
#endif

  if (type != SNES_PAD_NONE) {
#if SNES_PAD_DEBUG==true
    Serial.print("Device Type: ");
    Serial.print(type);
    Serial.print("\n");
#endif
    mouseX          = 0;
    mouseY          = 0;

    buttonA         = 0;
    buttonB         = 0;
    buttonX         = 0;
    buttonY         = 0;
    buttonStart     = 0;
    buttonSelect    = 0;
    buttonL         = 0;
    buttonR         = 0;

    directionUp     = 0;
    directionDown   = 0;
    directionLeft   = 0;
    directionRight  = 0;

  } else {
#if SNES_PAD_DEBUG==true
    Serial.print("Unknown Device.\n");
#endif
  }
}

void SNESpad::poll() {
  int32_t state = 0;

#if SNES_PAD_DEBUG==true
  Serial.print("SNESpad::poll: ");
#endif
      if (type != SNES_PAD_NONE) {
          state = read(); // polls current controller state

          if (state) {
              switch (type) {
                  case SNES_PAD_CONTROLLER:
                      directionLeft =  (state & SNES_LEFT);
                      directionUp =    (state & SNES_UP);
                      directionRight = (state & SNES_RIGHT);
                      directionDown =  (state & SNES_DOWN);
  
                      buttonSelect =   (state & SNES_SELECT);
                      buttonStart =    (state & SNES_START);
                      buttonB =        (state & SNES_B);
                      buttonY =        (state & SNES_Y);
                      buttonA =        (state & SNES_A);
                      buttonX =        (state & SNES_X);
                      buttonL =        (state & SNES_L);
                      buttonR =        (state & SNES_R);
  
                      break;
                  case SNES_PAD_NES:
                      directionLeft =  (state & SNES_LEFT);
                      directionUp =    (state & SNES_UP);
                      directionRight = (state & SNES_RIGHT);
                      directionDown =  (state & SNES_DOWN);
  
                      buttonSelect =   (state & SNES_SELECT);
                      buttonStart =    (state & SNES_START);
                      buttonB =        (state & SNES_Y);
                      buttonA =        (state & SNES_B);
  
                      break;
                  case SNES_PAD_MOUSE:
                      int x = 127;  //set center position [0-255]
                      int y = 127;
  
                      // Mouse X axis
                      x = (state & SNES_MOUSE_X) >> 25;
                      x = reverse(x) * SNES_MOUSE_PRECISION;
                      if (state & SNES_MOUSE_X_SIGN) x = 127 - x;
                      else x = 127 + x;
  
                      // Mouse Y axis
                      y = (state & SNES_MOUSE_Y) >> 17;
                      y = reverse(y) * SNES_MOUSE_PRECISION;
                      if (state & SNES_MOUSE_Y_SIGN) y = 127 - y;
                      else y = 127 + y;
  
                      mouseX  = x;
                      mouseY  = y;
                      buttonB = (state & SNES_X);
                      buttonA = (state & SNES_A);
  
                      break;
              }
  
  #if SNES_PAD_DEBUG==true
              if (_lastRead != state) {
                  Serial.print("Controller::A:"); Serial.print(buttonA);
                  Serial.print(", B:"); Serial.print(buttonB);
                  Serial.print(", X:"); Serial.print(buttonX);
                  Serial.print(", Y:"); Serial.print(buttonY);
                  Serial.print(", L:"); Serial.print(buttonL);
                  Serial.print(", R:"); Serial.print(buttonR);
                  Serial.print(", Select:"); Serial.print(buttonSelect);
                  Serial.print(", Start:"); Serial.print(buttonStart);
                  Serial.print(", Mouse::X:"); Serial.print(mouseX, BIN);
                  Serial.print(", Y:"); Serial.print(mouseY, BIN);
              }
              _lastRead = state;
  #endif
          } else {
              // device disconnected or invalid read
              type = SNES_PAD_NONE;
              start();
          }
      } else {
          start();
      }

#if SNES_PAD_DEBUG==true
  Serial.print("\n");
#endif
}

// init gpio pins
void SNESpad::init() {
#ifdef ARDUINO
  // Code specific to Arduino
  pinMode(clockPin,  OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(data0Pin, INPUT);
  pinMode(data1Pin, INPUT);
  pinMode(iobitPin, OUTPUT);

  digitalWrite(data0Pin, HIGH); // pull_up
  digitalWrite(data1Pin, HIGH);
#else
  // Code specific to Pico SDK
  gpio_init(clockPin);
  gpio_init(latchPin);
  gpio_init(data0Pin);
  gpio_init(data1Pin);
  gpio_init(iobitPin);

  gpio_set_dir(clockPin, GPIO_OUT);
  gpio_set_dir(latchPin, GPIO_OUT);
  gpio_set_dir(data0Pin, GPIO_IN);
  gpio_pull_up(data0Pin);
  gpio_set_dir(data1Pin, GPIO_IN);
  gpio_pull_up(data1Pin);
  gpio_set_dir(iobitPin, GPIO_OUT);
#endif

  return;
}

// signal mouse to go to next speed if not at desired speed
void SNESpad::setMouseSpeed()
{
  // default mouse to fastest speed
  if (type == SNES_PAD_MOUSE &&
      mouseSpeedFails < SNES_MOUSE_THRESHOLD &&
      mouseSpeed != SNES_MOUSE_FAST
     ) {

    custom_write(clockPin, 0);
    custom_delay_us(6);

    custom_write(clockPin, 1);
    custom_delay_us(12);
  }
}

// clock in a data bit
uint32_t SNESpad::clock(uint8_t datPin)
{
  uint32_t ret = 0;

  custom_write(clockPin, 0);
  custom_delay_us(12);

  ret = custom_read(datPin);

  custom_write(clockPin, 1);
  custom_delay_us(12);

  return ret;
}

// clock in data0/data1 dibits
uint32_t SNESpad::clock(uint8_t dat0Pin, uint8_t dat1Pin)
{
  uint32_t ret = 0;

  custom_write(clockPin, 0);
  custom_delay_us(12);

  ret = custom_read(dat0Pin) | ((custom_read(dat1Pin) & 1) << 1);

  custom_write(clockPin, 1);
  custom_delay_us(12);

  return ret;
}

// latch to start read
void SNESpad::latch()
{
  custom_write(latchPin, 1);
  custom_delay_us(12);

  setMouseSpeed();

  custom_write(latchPin, 0);
  custom_delay_us(12);
}

XbandKeyMapping SNESpad::getKeyFromScancode(uint8_t scancode, bool special) {
  XbandKeyMapping key = {"unused", 0, 0};

  if (special) {
    // Special Scancodes (preceded by SNES_KEY_SPECIAL scancode)
    switch (scancode) {
      case SNES_KEY_RETURN: // Unused alt Joy-A?
        key = {"JOY-A", KEY_RETURN, 1};
        break;
      case SNES_KEY_LEFT:
        key = {"LEFT", KEY_LEFT_ARROW, 1};
        break;
      case SNES_KEY_DOWN:
        key = {"DOWN", KEY_DOWN_ARROW, 1};
        break;
      case SNES_KEY_RIGHT:
        key = {"RIGHT", KEY_RIGHT_ARROW, 1};
        break;
      case SNES_KEY_UP:
        key = {"UP", KEY_UP_ARROW, 1};
        break;
    }
  } else {
    // Normal Scancodes
    key = keyMapping[(scancode & 0x0f)][((scancode & 0xf0) >> 4)];
  }

  return key;
}

bool SNESpad::setCapsLockLed(bool enabled) {
  capsLocked = enabled;
}

bool SNESpad::readKeyboard(bool readonlyID)
{
  uint8_t kid = 0;
  uint8_t num = 0;
  uint8_t i, n;

  // Activate the keyboard's host comm interrupt routine
  custom_write(iobitPin, 0);

  // Read the 8 bits (4 clocks) keyboard signature (id)
  for (i = 0; i < 8; i = i + 2) {
    uint32_t bits = clock(data0Pin, data1Pin); // clock shift bit in
    kid |= bits << i;

    if (i == 0 && readonlyID) {
      custom_write(iobitPin, 1); // KeyboardID read only (no scancodes)
    }

    // auto recovers common out of sync packet transaction
    if (i == 4 && kid == SNES_KEYBOARD_ID >> 2) {
      kid = kid << 2;
      break;
    }
  }

  // Can toggle caps lock after reading id
  if (!capsLocked) custom_write(iobitPin, 1); // Toggle Caps Lock LED

  // Read the 4 bits (2 clocks) upcoming scancode byte count (0-15)
  for (i = 0; i < 4; i = i + 2) {
    uint32_t bit = clock(data0Pin, data1Pin); // clock shift bit in
    num |= bit << i;
  }
  num = num & 0x0f; // number of scancodes ready (0-15)

  // Auto recover random bad ids
  if (type == SNES_PAD_KEYBOARD && kid == SNES_KEYBOARD_ID + 1) {
    kid = SNES_KEYBOARD_ID;
  }

  // Read the scancodes
  scancodes[16] = {0};
  if (!readonlyID && num && kid == SNES_KEYBOARD_ID) {
    for (n = 0; n < num; n++) {
      uint8_t readByte = 0;

      for (i = 0; i < 8; i = i + 2) {
        uint32_t bits = clock(data0Pin, data1Pin); // clock shift bit in
        readByte |= bits << i;
      }

      scancodes[n] = readByte;
    }
  }

  // End keyboard read
  custom_write(iobitPin, 1);
  scancodes_len = num;

#if SNES_PAD_DEBUG==true
  // log keyboard id and scancode length number
  Serial.print("KB_ID:0x");
  Serial.print(kid, HEX);
  Serial.print(" SCANCODES: ");
  Serial.print(num, DEC);
#endif

  // scancodes ready
  return kid == SNES_KEYBOARD_ID;
}

uint32_t SNESpad::read()
{
  uint32_t dat = 0;
  bool readonlyID = false;

  /* A connected device will pull the data line low prior to latch.
     A disconnected pin is kept high by internal pull_up.*/
  uint32_t disconnected = custom_read(data0Pin);

  // normal 16-bit controller read (unused)
  latch();
  for (uint8_t i = 0; i < 32; i++) {
    uint32_t bit = clock(data0Pin); // clock shift bit in
    dat |= bit << i;

    if (i == 15) {
        bool read_extra = !bit; // check if mouse
        if (!read_extra) break; // skip extra bytes if not
        custom_delay_us(12);
    }
  }

  // check and read keyboard
  bool isKeyboard = readKeyboard(readonlyID);

  dat = ~dat; // ctrlr buttons are active low, so invert bits

  // verify controller or mouse is connected
  if (!isKeyboard && disconnected && !(dat & 0xffff)) {
      type = SNES_PAD_NONE;
      mouseSpeedFails = 0;
      mouseSpeed = 0;
      return 0;
  }

  // check device type id
  bool isSNES = ((dat & SNES_DEVICE_ID) >> 12) == SNES_PAD_ID;
  bool isMouse = ((dat & SNES_DEVICE_ID) >> 12) == SNES_MOUSE_ID;
  bool isNES = ~dat && ((dat >> 8) & 0xff) == 0xff  ;

  // verify mouse speed
  if (isKeyboard) {
      type = SNES_PAD_KEYBOARD;
  } else if (isSNES) {
      type = SNES_PAD_CONTROLLER;
  } else if (isNES) {
      type = SNES_PAD_NES;
  } else if (isMouse) {
      uint8_t lastMouseSpeed = mouseSpeed;

      // parse mouse speed bits
      mouseSpeed = ((dat & SNES_MOUSE_SPEED) >> 10);
      if (mouseSpeed > 2) mouseSpeed = 0;

      // detect hyperkin mouse failure to change speed to halt further attempts
      if (
          mouseSpeed != SNES_MOUSE_FAST
          && lastMouseSpeed == mouseSpeed
          && mouseSpeedFails < SNES_MOUSE_THRESHOLD
      ) {
          mouseSpeedFails++;
      }

      type = SNES_PAD_MOUSE;
  } else {
      type = SNES_PAD_NONE; //UNKNOWN device id

      #if SNES_PAD_DEBUG==true
        Serial.print("UNKNOWN\n");
      #endif
  }

  return dat;
}

// reverse bits within a byte (ex: 0b1000 -> 0b0001)
uint8_t SNESpad::reverse(uint8_t c) {
  char r = 0;
  for (int i = 0; i < 8; i++) {
    r <<= 1;
    r |= c & 1;
    c >>= 1;
  }
  return r;
}
