#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

inline bool getBit(uint16_t b, uint8_t n) __attribute__((always_inline)); //inline in order to improve speed

/******Initialisation functions*******/
void initRelays(void);
void initLCD(void);
void initHallSensors(void);
void initLEDs(void);
void initBuzzer(void);
/******Initialisation functions*******/

/******Loop functions******/
void relaysOn(void);
void relaysOff(void);

uint16_t readHallSensors(void);
boolean evaluateMotor(uint16_t reading, char* s);

void displayNewReadingLCD(boolean comboFound, uint16_t reading, char* combinationName);
void displayNewReadingLED(boolean comboFound, uint16_t reading);

void displaySameReadingLCD(void);
void displaySameReadingLED(void);

void buzzerCorrect(void);
void buzzerIncorrect(void);

void writeToSD(unsigned long t, boolean comboFound, uint16_t reading, char* combinationName);
/******Loop functions******/

#endif
