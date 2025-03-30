# R2A15218FP_R2A15220FP_Arduino_library

A library for controlling the R2A15218FP and R2A15220FP audio switch
and volume control.

The parts have six (6) 21 bit registers, and use a funny serial scheme,
so not SPI or I2C.  Data bits must be valid on the rising clock; data
line must be low at fall of clock <b>except for</b> the 24th bit, in which
case data line must be high at falling clock, which latches the 24 bits
into the appropriate register.  Register address is the last three bits
of the 24 bit stream.  There is no chip select pin, so this must be a
dedicated interface.  Needless to say, this is all implemented with
bit-banging.

One clever bit is the use of bitfields to talk about the different
pieces of each register, and a union with a 32 bit integer for purposes
of transmission.

Also: experimentation reveals some functions that don't exist (or are
not documented) in the R2A15220FP, which I have brought out.  These
enable signal to be routed to the surround (SB) outputs either direct
from the main mux, or from the front outputs (so, after volume control.)
The other "new" feature is an enable to get Record Out working.
"set_enable_rec_out" is used with "set_recN" to enable record out N.

NOTE: You should mute the audio output (outside and independent of
the R2A15218FP chip) for about 400msec when switching inputs lest you
get a speaker or ear damaging POP.  Or so says the guy (geo98) on
the internet who gave other valuable insight about these parts in a forum.

Tested with ESP32, but should work with other processors.

Bill Dudley
2025 03 30
