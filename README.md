# Bare Metal RPI WAV player

WAV file player with song navigation, queueing capabilities, and interactive play/pause/restart.

ONLY works with 16-bit WAV files (plays back is 8bit).

Interface remains slow and still has buggy queue indication.

Curerntly interface is set to handle less than 8 songs while maintaining interface integrity.
However, the ability to play music can extend beyond this(maximum # of songs SD card can handle).

Video of semi working model(Didn't have a soldering kit so audio was awful but output for the pins was accurate): https://youtu.be/end3gPYIK9g

# Refrences

Information on reading WAV files: http://www.topherlee.com/software/pcm-tut-wavformat.html

Reading from SD card: http://elm-chan.org/fsw/ff/00index_e.html

