# DOSI Laser control firmware

## Issues

For some reason the I2C signals are not wired to the TWI pins on the MCU.
Using bit-bang I2C code from Wikipedia.

## Functional Description

This is firmware (C code) for the microcontroller on the isolated I2C
master board which controls the lasers in a DOSI box.

![image](images/board.jpg)

The firmware accepts control commands from either a USB/serial adapter
(connected to a host computer via USB) or the Zynq embedded computer
in the DOSI box.  The protocol used is asynchronous serial (UART) with
the following settings:

* 1200 Baud
* 8 data bits
* 1 stop bit
* no parity

The firmware will send at power-up a brief message including the
firmware version, followed by a prompt "> ".

Commands consist of an ASCII text string followed by a CR and/or LF
character.  Each command will produce a response, which consists of
zero or more lines of text, followed again by the prompt "> ".

Line editing supported:
   DEL or Backspace will delete and move cursor left
   ^P will recall the last line

Each command consists of a single alphabetic character, followed by
zero or more space-separated arguments.  The arguments may be decimal
numbers, or hex numbers with "0x" prefix.

Here is a preliminary list of commands:

    H       - help
    X       - debug read
    E l     - enable
    D [l]   - disable
    L v     - set LEDs
    I l mv  - set laser VSET in mv
    N l mv  - set laser VSET (non-volatile)
    C       - clear I2C errors
    S l     - select I2C bus l
    P l     - dump digital pot
    P l r v - set digital pot reg
    R l     - read laser ADC

A few notes:

```E``` (enable) can take either an integer 0-5 or a 6-digit binary string.

```L``` (LEDs) sets the state of two LEDs on the control board for debugging.
<br>A value of (0, 1, 2, 3) corresponds to (none, LED1, LED2, both) on.

```I``` and ```N``` expect a value in mv (0-2500).  There is some
error due to the integer conversion from 2500 to 255 and also the ADC
quantization.  The value of VSET read using the ADC is pretty
accurate.

```IMON``` as displayed is converted to laser mA assuming a 4.7 ohm
sense R.

## Calibration / conversions

Here are some assumptions built into the code:

Lasers are operated in "constant power" mode.  The power is set by supplying
a voltage to VSET, where the conversion is:

    I(PD) = VSET / (2 * Rpd)     where Rpd is either 499 or 4999 ohms (jumper)

Ideally the software would know about this and the current would be
set directly in uA of photodiode current.  For now we assume the 499
ohm setting which corresponds to a 100x factor.  So, VSET is set in mV
from the command line which correspond directly to uA of photodiode
current.

IMON is the voltage on the monitor current.  The laser current is:

    I(LD) = IMON / (2 * Rpd)    where Rpd is 4.7 so
	      = IMON / 9.4 =  IMON * 5 / 47




