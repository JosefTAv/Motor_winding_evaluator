#include "peripherals.h"
#include "constants.h"
//#define DEBUG

volatile boolean activated = false;

void enableSignalISR(void);
void initActivationPin(int enablePin);

void setup() {
  Serial.begin(115200);
  delay(50);

  //initialise IO peripherals
  bool SDWorking = initSD();
  initLCD(SDWorking);
  initHallSensors();
  initLEDs();
  initRelays();
  initActivationPin(ENABLE_PIN);
}

uint16_t oldMeasurement = 0;
uint8_t comboIndex = nbCombinations;
uint16_t k = 0;

void loop() {
  if (activated) {
    delay(50); //Check that signal really is LOW
    if(digitalRead(ENABLE_PIN) == 0){
      displayStartMeasure();
      relaysOn(); //Allow current to flow to create magnetic field
      delay(1200); //Wait for relays to turn on before making a measurement, could be longer (than 1000) because it doesn't reach full current yet. But the measurements look good

      unsigned long measureTime = millis();
      uint16_t measurement = readHallSensors();
      relaysOff();
      if(measurement != oldMeasurement){
        comboIndex = evaluateMotor(measurement);
        displayNewReadingLCD(comboIndex, measurement);
        displayNewReadingLED(comboIndex, measurement);
        
        oldMeasurement=measurement;
      }
      else{
        displaySameReadingLCD();
        displaySameReadingLED();
      }
      (comboIndex == CORRECT) ? buzzerCorrect() : buzzerIncorrect(); 
      writeToSD(measureTime, comboIndex, measurement);
      delay(10); // When the relays turn off, they create a bounce in the signal which activates measurement again and never stops: wait for bounce then continue
      activated = false;
      k++;
    }
  }
}

void enableSignalISR(void) {
    if(digitalRead(ENABLE_PIN) == 0) { //Check that the downward flank is not noise, check thrice for bouncing
      activated = true;
  }
}

void initActivationPin(int enablePin){
  pinMode(enablePin, INPUT_PULLUP); //Init activation signal pin
  attachInterrupt(digitalPinToInterrupt(enablePin), enableSignalISR, FALLING); //interrupt for the enable signal
}
