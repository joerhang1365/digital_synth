# DIGITAL_SYNTH

# LIBRARIES

PortAudio

Some Sound card

# HOW TO USE

This digital synth only works under linux right now.
It uses linux audio drivers to ouput sound and uses linux devices to fetch midi data.
So if you use windows or mac idk what to say.

1. Start by compiling the source code.
   Install Makefile and run the "make" command inside of the directory to the "makefile" file.

2. Run the executable using"./digital_synth".

3. Select available device.

4. Enter path to midi device controller.
   The path to midi device controller is usually found in the "/dev" directory on linux.

5. Slam keys and make music.
   The program is still kinda buggy and sometimes notes do not turn off unless you hit them twice.
   I might fix that.
   There also is no way to turn the program off besides typing "CTRL+Z". 
