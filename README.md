# Motor winding evaluator

## Introduction

This contains the code for my **final year bachelor project** which aims to verifiy the windings of a motor. This is done by measuring the polarity of the magnetic poles of the motor between each rotor when a supplied a certain amount of current. A correctly wound motor will produce a specific sequence of magnetic pole directions. An incorrectly wound rotor can be detected based on which poles do not coincide with this sequence.
Notably, the machine considers the motor to be correct when the **polarities are in this order**: &uarr; &darr; &uarr; &darr; &darr; &uarr; &darr; &uarr; &darr; &uarr; &uarr; &darr; 

### Input ports
The device has several input ports:
- Two switches on the **front panel**: one for turning on the machine and one for activating it.
- A power port on the **back panel**
- A 12 pin plug on the **back panel**, where the motor mount is connected
- A 4 pin plug on the **back panel**, where the external controller/computer is connected
- Another 4 pin plug on the **back panel**, where the LEDs can be plugged in
- A slot where a micro SD card can be plugged in



### Feedback devices
The machine provides feedback for the motor in several ways: 
- An LCD stating if the windings are correct or which windings are incorrect
- An LED ring which displays green when a rotor has the right polarity and red when it does not
- A buzzer producing a different sound when it is wound correctly and not
- An SD card where each measurement is logged so as to reduce the need for a technician to manually note down each test.

To use this code, you simply need to upload it to an Arduino Mega using Arduino IDE and connect each peripheral based on the pin definitions found in "constants.h". These definitions can be changed so long as they are exchanged for pins with similar functionalities.

## Operation of the machine
To turn it on, the rightmost switch must be flipped up and the switch at the back of the machine must be on. The display will show that no measurements have been as well as a warning if the SD card is not working. Then the motor needs to be placed on the mount in the proper orientation i.e. winding A1 must be aligned with the blue marker.
### Activation
The motor can be evaluated in one of two ways: 
1. Using the left toggle switch to activate the measurement
1. A measurement can also be activated by a downward flank through the external controller port.

### Feedback
Once activated, the motor will be supplied current and the direction of the poles between each rotor will be measured. The poles will be measured anti-clockwise from A1 until it makes a full circle. After this, the lcd will display each of these polarities and whether they are correct or not with either a ✓ (for correct ones) or a ✗. The leftmost pole on the lcd represents the first one: A1. The device will then check the combinations of these poles to analyse which windings are inverted and print this under the field &quot;state:&quot;. The LEDs will also go green if the pole direction is correct and red if incorrect. The buzzer will have a distinct sound for when the poles are entirely correct and for when they are not.

In the case where the next measurement is the same as the last, two things will happen. Firstly the lcd will print &quot;same as previous&quot; and secondly the LEDs will blink. Otherwise, the operation will be identical.

Importantly, in every measurement, once it is completed the device will **stop supplying current** to the motor and one of the pins on the connector that goes to the computer will **be pulled down** so as to communicate that the measurement is finished.

Each measurement will also be logged to the SD card (if it is inserted and working). The fields of each measurement will be:
1. The number of the measurement
1. The time at which the measurement was taken
1. Whether the motor was correct or not
1. The measurement of the poles themselves
1. Which poles were correct and incorrect (represented in binary)
1. The winding type


