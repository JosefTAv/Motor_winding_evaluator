#include "peripherals.h"
#include "constants.h"
//#define DEBUG

#define id(x) #x //return name of variable

volatile boolean activated = false;

void enableSignalISR(void);

void setup() {
  Serial.begin(115200);
  delay(50);

  //initialise IO peripherals
  initLCD();
  initHallSensors();
  initLEDs();
  initRelays();
  pinMode(ENABLE_PIN, INPUT_PULLUP); //Init activation signal pin
  attachInterrupt(digitalPinToInterrupt(ENABLE_PIN), enableSignalISR, FALLING); //interrupt for the enable signal
}

uint16_t oldReading = 0;

void loop() {
  if (activated && digitalRead(ENABLE_PIN) == 0) {
    relaysOn();
    delay(2000);
    
    uint16_t reading = readHallSensors();
    if(reading!=oldReading){
      oldReading=reading;
      char combinationName[100];
      boolean comboFound = evaluateMotor(reading, combinationName);
      displayNewReadingLCD(comboFound, reading, combinationName);
      displayNewReadingLED(comboFound, reading);
      (combinationName == "Correct") ? buzzerCorrect() : buzzerIncorrect(); 
    }
    else{
      displaySameReadingLCD();
      displaySameReadingLED();
    }

    relaysOff();
    activated = false;
  }
}

void enableSignalISR(void) {
  activated = true;
}
