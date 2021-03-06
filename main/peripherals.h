#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "constants.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

inline bool getBit(uint16_t b, uint8_t n) __attribute__((always_inline)); //inline in order to improve speed

//Template/Macro for counting number of elements in array safely, works for many types
template <typename T, size_t N>
char ( &_ArraySizeHelper( T (&arr)[N] ))[N];
#define LENGTH( arr ) (sizeof( _ArraySizeHelper( arr ) ))

extern uint16_t nbCombinations;

/******Initialisation functions*******/
void initRelays(void);
String checkFileNamesSD(void);
bool initSD(String fileName);
void initLCD(void);
void initMessageLCD(bool SDWorking, String fileName);
void initHallSensors(void);
void initLEDs(void);
void initBuzzer(void);
/******Initialisation functions*******/

/******Loop functions******/
void motorOn(void);
void motorOff(void);
void errorPortOn(void);
void errorPortOff(void);

uint16_t readHallSensors(void);
uint16_t evaluateMotor(uint16_t reading);

void displayStartMeasure(void);
void displayEndMeasure(void);
void displayNewReadingLCD(uint8_t comboIndex, uint16_t reading);
void displayNewReadingLED(uint8_t comboIndex, uint16_t reading);

void displaySameReadingLCD(uint8_t comboIndex, uint16_t reading); //blink screen with "Same as previous"
void displaySameReadingLED(void);

void buzzerCorrect(void);
void buzzerIncorrect(void);

void writeToSD(String fileName, unsigned long t, uint8_t comboIndex, uint16_t reading);
/******Loop functions******/

#endif
