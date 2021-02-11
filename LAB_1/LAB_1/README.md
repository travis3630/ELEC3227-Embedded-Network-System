IlMatto RFM12B setup
-------------
This code provides the starting point for communicating using the rfm12b radio.
This minimal code does not actually transmit or receive anything.

The radio is configured using the settings in `rfm12b_config.h` so that it
works with the breakout board.
The radio is setup with `rfm12_init()` to use the 433MHz band, 30byte packets
at 115kbaud.
The library must be located in the `librfm12/` subdirectory (as specified by includes in `rfm12.h`).
It is configured to use a Pin change interrupt on PIN B1 to capture interrupts
from the radio.
