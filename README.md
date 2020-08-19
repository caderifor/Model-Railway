# Model-Railway
Arduino sketches used for model railway control projects.

First GitHub upload is EBTS - Electronic Block Token System for my 0-16.5 / 7mm scale narrow gauge model railway.   
The idea is to model a system where a train may not enter a single line section unless the driver is in posession of a token.    
Three block instruments are interlocked by an Arduino so that tokens are locked in the machines if the block section is not free.   
The system is working with the Arduino controlling sections of line with three machines and a virtual machine for the off scene section of railway, with just two push buttons.

First projects were control of trains on an "N" gauge model layout by means of a TV type remote control.
The programme simulated train weight, regulator opening and/or brake application to give speed controlled by inertia simulation.  This is working .  
Loco drive uses the PWM output of the Arduino.   
This project works but the acceleration / inertia calculations could be refined.

This project linked to one that played a loco whistle sound stored in the memory of an Arduino Mega and a home built D to A ladder converter.  Working, but not as efficient as using an SD Card reader would be.  See UnrealTime clock below.

Other projects on the go are motor, servo and stepper motor projects for level crossings, a lifting bridge and PIR controlled opening door for a station foyer.   
Station crossing plus PIR door opening Working.  
Lifting bridge code written but hardware still to do.

The latest project is the  "Unreal Time Clock"  that can be used to run to a timetable but fast forward through the times when nothing is happening.   Control uses the same TV remote control system as the train controller.
Having this interface, the clock includes an SD card reader loaded with 30 railway sounds, controlled by the Remote, that can be played out to speakers at either or both stations on the line.
The original plan was to hack an analogue clock, but they reacted badly to attempts to speed them up.  A switch was made to digital, but TFT screens did not work out.  The project is nearing completion (or rather, crucial testing) using a second 
Arduino taking timing pulses from the master and deriving time to be displayed on multiplexed seven segment displays.
To obtain big enough numerals and a circuit board, a DIY kit digital clock is being hacked.
