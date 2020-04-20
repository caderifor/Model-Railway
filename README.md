# Model-Railway
Arduino sketches used for model railway control projects.

First GitHub upload is EBTS - Electronic Block Token System for my 0-16.5 / 7mm scale narrow gauge model railway.   
The idea is to model where a train may not enter a single line section unless the driver is in posession of a token.    
Three block instruments are interlocked by an Arduino so that tokens are locked if the block section is not free.   
The components of the system are working with the Arduino, but the overall problem does not work yet.

First projects were control of trains on an "N" gauge model layout by means of a TV type remote control.
The programme simulated train weight, regulator opening and/or brake application to give speed controlled by inertia simulation.  Working .  Loco drive uses the PWM output of the Arduino.   
This project linked to one that played a loco whistle sound stored in the memory of an Arduino Mega anda home built D to A ladder converter.  Working, but not as efficient as using an SD Card reader would be.  Future project maybe?
This project works but the acceleration / inertia calculations could be refined.

Other projects on the go are motor, servo and stepper motor projects for level crossings, a lifting bridge and PIR controlled opening door for a station foyer.   
One crossing plus door opening Working.  

Lifting bridge code written but hardware still to do.

One idea for the future is an "Unreal Time Clock" so that a wall clock can be used to run to a timetable but run fast forward through the times when nothing is happening.   
Control will use the same TV remote control system as the train controller.
Since this would not overtax the Arduino, I want to include a SD card reader sounds player (controlled by the Remote)
