- Think which pins or parts we loose access if we make a stack up board that covers the caravel board: J2 serial toggle, blinking leds, reset button?
- For now leave video output as just pins (no VGA connector or driver chip). Maybe prepare some pins to do an add on board for video? 
- ~~Put some jumpers (or some switches) on the SRAM CS pin, to be able to use another device (an fpga) instead of the memory~~
- ~~Connect caravel `gpio` pin to a LED?~~
- ~~Change J2 connection on caravel board to female header and male pins on this shield board~~
- ~~Add a row of test pins or headers connected to ground~~
- ~~J2 serial toggle (I could put some female header on the bottom of the board to connect to J2). I could also add a switch to change between modes~~
- ~~Use long "double sided" male header pins to have access to all the pins for debugging or connecting other stuff~~
- Can I make the board shorter if it's just from memories? (no video output or gpio )
- ~~Connect IO[26]-HACK_EXTERNAL_RESET to a pull down and a switch~~

Future versions:
- Put VGA connector + driver + resistors
- Put LEDs for GPIOs appart from header pins?
- Think how to connect an fpga (icebreaker?) for debugging memories communication and fake programs?
- Add caravel button to caravel RST_N ? 
- PS2 keyboard to caravel connector?


