# XBand Keyboard Protocol

The XBand keyboard data is read by the console using both DATA lines on pins 4 and 5 as well as the IOBBIT on pin 6. After a normal 16-bit controller read, the XBand keyboard begins communication when the IOBIT line is set low. Then data bits are clocked in from both data lines simultaneously. So for example, the first clock pulse would read in bits 0 and 1 from DATA lines 0 and 1. Continuing until the keyboard scancode buffer read is completed and the IOBIT line is set back to high.

The keyboard data read starts with an 8-bit keyboard_id of 0x78. Then the next 4-bits are a number between 0-15, indicating the number of scancode bytes to be clocked-in.

Each keyboard key press is reported as unique scancode byte. Normal keys just report their single unique 8-bit scancode. While special and meta keys report an extra scancode to indicate when the key is released (0xf0). Additionally special keys are preceeded by a special key scancode (0xe0).

Normal keys:
nn (pressed)

Meta keys: (ctrl, alt, shift, capslock, xband, joy-pad keys)
nn (pressed)
f0, nn (released)

Special keys: (up, down, left, right)
e0, nn (pressed)
e0, f0, nn (released)

## Normal/Meta Key Scancode Table (without prefix)

 ____0xh___1xh___2xh___3xh___4xh___5xh___6xh___7xh_____8xh_____9xh___
  x0h ---   ---   ---   ---   ---   ---   ---   NUM-0   OPENX   ---
  x1h F1    LALT  C     N     ,<    ---   ---   NUM-.   CLOSEDX ---
  x2h F2    LSHFT X     B     K     '"    ---   NUM-2   ---     ---
  x3h F3    ---   D     H     I     ---   ---   NUM-5   ---     ---
  x4h F4    LCTRL E     G     O     [{    ---   NUM-6   NUM-SUB ---
  x5h F5    Q     4$    Y     0)    =+    ---   NUM-8   ---     ---
  x6h F6    1!    3#    6^    9(    ---   BS    ESC     JOY-A   ---
  x7h F7    ---   ---   ---   ---   ---   ---   NUM-DIV JOY-B   ---
  x8h F8    ---   ---   ---   ---   CAPS  ---   ---     JOY-X   ---
  x9h F9    ---   SPACE ---   .>    SHF2? NUM-1 NUM-RET JOY-Y   ---
  xAh F10   Z     V     M     /?    ENTER ---   NUM-3   JOY-L   ---
  xBh F11   S     F     J     L     ]}    NUM-4 ---     JOY-R   ---
  xCh F12   A     T     U     ;:    \|    NUM-7 NUM-ADD SELECT  ---
  xDh TAB   W     R     7&    P     \|    ---   NUM-9   START   ---
  xEh \`~    2@    5%    8*    -_    ---   ---   NUM-MUL ---     ---
  xFh ---   ---   ---   ---   ---   ---   ---   ---     ---     ---

## Special Scancodes (with E0h-prefix)
 E0h,5Ah  JOY-A (alternate to normal scancode 86h)
 E0h,6Bh  LEFT
 E0h,72h  DOWN
 E0h,74h  RIGHT
 E0h,75h  UP

## Read Keyboard ID Only

By raising the IOBIT line after reading the first clock cycle of the 8-bit keyboard id byte it will indicate to the keyboard to return only the id and no scancodes. This is useful for checking if a keyboard is connected without needing to read in any scancodes.

## Caps Lock Toggle

By setting the IOBIT line high after reading the keyboard ID, the caps lock LED can be enabled. Keeping the IOBIT line low until the end of the keyboard read will disable the caps lock LED.

## Resources
 - https://github.com/gilligan/snesdev/blob/master/docs/fullsnes.txt#L4069
 - https://git.agiri.ninja/natalie/xband-source