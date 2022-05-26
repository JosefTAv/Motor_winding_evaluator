# Motor winding evaluator
This contains the code for my final year bachelor project which aims to verifiy the windings of a motor by observing the polarity of the magnetic poles of the motor between each rotor when a certain amount of current is pulsed. A correctly wound motor will produce a certain sequence of poles. The incorrectly wound rotors can be detected based on which poles are inverted or out of sequence.

The machine provides feedback for the motor in several ways: an LCD stating if the windings are correct or which windings are incorrect, an LED ring which displays green when a rotor has the right polarity and red when it doesn't, a buzzer producing a different sound when it is wound correctly and not, and finally the results will be logged to an SD card to as to reduce the need for a technician to manually note down each test.

To use this code, you simply to to upload it to an Arduino Mega using Arduino IDE and connect each peripheral based on the definitions found in "constants.h".
