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
  initLCD();
  initHallSensors();
  initLEDs();
  initRelays();
  initActivationPin(ENABLE_PIN);
}

uint16_t oldMeasurement = 0;
uint8_t comboIndex = nbCombinations;

void loop() {
  if (activated) {
    relaysOn(); //Allow current to flow to create magnetic field
    delay(2000); //Wait for relays to turn on before making a measurement
    
    uint16_t measurement = readHallSensors();
    if(measurement != oldMeasurement){
      comboIndex = evaluateMotor(measurement);
      displayNewReadingLCD(comboIndex, measurement);
      displayNewReadingLED(comboIndex, measurement);
      (comboIndex == CORRECT) ? buzzerCorrect() : buzzerIncorrect(); 
      writeToSD(millis(), comboIndex, measurement);
      
      oldMeasurement=measurement;
    }
    else{
      writeToSD(millis(), comboIndex, measurement);
      displaySameReadingLCD();
      displaySameReadingLED();
    }

    relaysOff();
    activated = false;
  }
}

void enableSignalISR(void) {
    if(digitalRead(ENABLE_PIN) == 0) { //Check that the downward flank is not noise
    unsigned long start = millis();
    while(millis() - start < 200) //For testing length of tie required to determine proper activations
      Serial.println(digitalRead(ENABLE_PIN));
    activated = true;
  }
}

void initActivationPin(int enablePin){
  pinMode(enablePin, INPUT_PULLUP); //Init activation signal pin
  attachInterrupt(digitalPinToInterrupt(enablePin), enableSignalISR, FALLING); //interrupt for the enable signal
}
