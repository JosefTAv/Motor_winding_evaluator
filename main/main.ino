#include "peripherals.h"
#include "constants.h"
//#define DEBUG

volatile boolean activated = false;

void enableSignalISR(void);
void initActivationPin(int enablePin);

String fileName = "";
bool SDWorking = false;

void setup() {
  Serial.begin(115200);
  delay(50);

  //initialise IO peripherals
  fileName = checkFileNamesSD();
  SDWorking = initSD(fileName);
  initLCD(SDWorking, fileName);
  initHallSensors();
  initLEDs();
  initRelays();
  initActivationPin(ENABLE_PIN);
}

uint16_t oldMeasurement = 0;
uint8_t comboIndex = nbCombinations;

void loop() {
  if (activated) {
    delay(50); //Check that signal really is LOW
    if(digitalRead(ENABLE_PIN) == LOW){ //DOWNWARD Flank
      displayStartMeasure();
      motorOn(); //Allow current to flow to create magnetic field
      delay(1200); //Wait for relays to turn on before making a measurement, could be longer (than 1000) because it doesn't reach full current yet. But the measurements look good

      unsigned long measureTime = millis();
      uint16_t measurement = readHallSensors();
      motorOff(); //Deactivate motor to conserve power
      Serial.println(measurement, BIN);
      if(measurement != oldMeasurement){ //Check if current measurement is the same as previous
        comboIndex = evaluateMotor(measurement);
        displayNewReadingLCD(comboIndex, measurement);
        displayNewReadingLED(comboIndex, measurement);
        
        oldMeasurement=measurement;
      }
      else{
        displaySameReadingLCD();
        displaySameReadingLED();
      }
      if(comboIndex == CORRECT){ // check if the motor is correct or not
        buzzerCorrect(); 
        errorPortOff(); //Not necessary but good for redundancy
      }
      else{
        buzzerIncorrect();
        errorPortOn();
      }
      
      if(SDWorking) 
        writeToSD(fileName, measureTime, comboIndex, measurement);
    }
    else{
      delay(50); //Debounce
      if(digitalRead(ENABLE_PIN) == HIGH) //UPWARD Flank
        errorPortOff(); //reset error pin
    }
    delay(10); // When the relays turn off, they create a bounce in the signal which activates measurement again and never stops: wait for bounce then continue
    activated = false;
  }
}

void enableSignalISR(void) {
  activated = true;
}

void initActivationPin(int enablePin){
  pinMode(enablePin, INPUT_PULLUP); //Init activation signal pin
  attachInterrupt(digitalPinToInterrupt(enablePin), enableSignalISR, CHANGE); //interrupt for the enable signal
}
