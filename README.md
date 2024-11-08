# Model-Railway
Arduino sketches used for model railway control projects.

Latest project - PWM_loco_sim_controller.ino -  Steam Train simulator / controller for 12V DC trains (Analogue - not DCC)    PWM drive to locos.  Simulation of driving a steam loco with pots for Regulator, Gear, Brake  (application and release), train weight, and a "custom" value to cut delay time before different motors start turning.  The analogue values are combined in the Arduino Uno to determine acceleration or deceleration and provide the PWM signal.   
LED indication of  Emergency Stop, mid gear, brake off point, brake (proportional) indicator, Wheel slip warning, custom (start boost) and SIMPLE mode on back panel.  Hardware - 1 Arduino Uno, a voltage comparator and a bipolar transistor power amplifier with overcurrent protection to give 12Volt PWM drive power - Direction is switched by a relay (from the Gear pot) after voltage and current meters.   Emergency Stop button provided.
Because model locos start moving at different voltages and can cause long embarrasing delays before moving off, another "custom" pot can be set to allow sluggish locos to skip the long delay.

Previous project ( Bunny corner) was devised to give fun animations in one corner of the layout.  When a train enters the area (from either direction), three rabbits dissapear down their burrows (driven by three servos wired in parallel), a model me standing in front of my caravan waves to the train  and just before the rabbits creep out after the train has gone, a red kite circles round a nearby hilltop propelled by a small stepper motor.   (Arduino UNO and motor shield - two LDRs  buried between the rails - one LDR buried in the top of a tree, where the kite perches)

Previous  project is the  "Unreal Time Clock"  that can be used to run to a timetable but fast forward through the times when nothing is happening.   Control uses the same TV remote control system as the train controller.
Having this interface, the clock includes an SD card reader loaded with 30 railway sounds, controlled by the Remote, that can be played out to speakers at either or both stations on the line.
The original plan was to hack an analogue clock, but they reacted badly to attempts to speed them up.  A switch was made to digital, but TFT screens did not work out.  The project was completed using a second 
Arduino taking timing pulses from the master and deriving time to be displayed on multiplexed seven segment displays.
To obtain big enough numerals and a circuit board, a DIY kit digital clock was hacked.

Previously The BRIDGE project motorized a model Rolling Bascule Bridge  (3D print files on Thingiverse) with a Nema 17 stepper motor lifting the bridge, 2 servo motors driving the adjacent level crossing gate and two DC motor outputs driving a Red/Green rail signal and alternate reds for warnings before gates close.  These are all handled by the Motor Shield ( two LM293 chips)
These lights are pairs of LEDs wired in opposite polarity, plus a limiting resistor.  Blueish white LEDs on the bridge model Xenon beacons flashed, via a Mosfet on Pin2, before the bridge moves. There were no pins free to drive a small speaker with the warning tones, so a Nano was used to drive a speaker with two tones by monitoring the polarity of the flashing lights (Motor) output.  (See neenaw_addon.ino) Opto isolators drive one or other Nano pins, depending on polarity, while the lights are flashing.

Before that  -  EBTS - Electronic Block Token System for my 0-16.5 / 7mm scale narrow gauge model railway.   
The idea is to model a system where a train may not enter a single line section unless the driver is in posession of a token.    Three block instruments are interlocked by an Arduino so that tokens are locked in the machines if the block section is not free.   
The system is working with the Arduino controlling sections of line with three machines and two buttons making a virtual machine for the off scene section of railway.

Before that  -  Motor_shield_tester was run up to check the motor shield.  It exercise the outputs and they are monitored on reversed pairs of LEDs on all four motor outs.  It gave a nice insight into what the stepper motor does.

Before that  -  The STATION sketch has two servos to oerate crossing gates and PIR controlling a butchered CD head drive that opens the foyer door of the station as visitors approach.    The latest version has been rewritten in procedures.

First projects were for control of trains on an "N" gauge model layout by means of a TV type remote control.
The programme simulated train weight, regulator opening and/or brake application to give speed controlled by inertia simulation.   Loco drive uses the PWM output of the Arduino via a previously built DC Amplifier.   
This project works but the acceleration / inertia calculations could be refined.
This project linked to one that played a loco whistle sound stored in the memory of an Arduino Mega with a home built D to A ladder converter.  Working, but not as efficient as using an SD Card .  (See UnrealTime clock - above)

