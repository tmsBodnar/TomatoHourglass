# TomatoHourglass
Hourglass and Tomato-timer with arduino


I made this timer for my efficient work.

###Parts
The Tomato Hourglass built from:
* Arduino nano
* 4 x MD MAX7200 8x8 Led MAtrix
* 9 DOF Orientation Sensor
* RTC Real Time Clock module

###Features
The Tomato Hourglass have the following features:
* Mesaures work (25 min), short break (5 min), long break (10min) times
* Three position of orientation:
* * Normal clock 
* * Work time measuring
* * Break time measuring

-----
###Functions:
In Normal position shows the digital clock with 24H format
In Standing position firstly starts measuring the work time. 
* the remaining time shown
* shown a hourglass animation also
If work time ends, blinking to set up upside down
After turning, measure the break time, first short break, then again, and third time a long break.
Can allways reset the Tomato Timer to put the device in normal position (clock)

----
Todos:
needs a 3D printed case
needs to bugfixing in arduino code
