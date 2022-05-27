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
  if (activated) {
    relaysOn(); //Allow current to flow to create magnetic field
    delay(2000); //Wait for relays to turn on before making a measurement
    
    uint16_t reading = readHallSensors();
    if(reading!=oldReading){
      oldReading=reading;
      uint16_t comboIndex = evaluateMotor(reading);
      displayNewReadingLCD(comboIndex, reading);
      displayNewReadingLED(comboIndex, reading);
      (comboIndex == 0) ? buzzerCorrect() : buzzerIncorrect(); 
      writeToSD(millis(), comboIndex, reading);
    }
    else{
      writeToSD(millis(), comboIndex, reading);
      displaySameReadingLCD();
      displaySameReadingLED();
    }

    relaysOff();
    activated = false;
  }
}

void enableSignalISR(void) {
  if(digitalRead(ENABLE_PIN) == 0) //Check that the downward flank is not noise
    activated = true;
}
