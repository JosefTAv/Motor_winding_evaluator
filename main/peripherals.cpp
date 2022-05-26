#include "peripherals.h"
#include "constants.h"
#include "pitches.h"

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line 
Adafruit_NeoPixel LEDstrip(NB_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); //12 = NB_HALL_SENSORS
uint16_t nbMeasurements = 0;

/*******CONSTANTS*******/
//Known polarity combinations
//Each bit represents a polarity: 1-> N pole, 0-> S pole
//LSb represents the measurement of the last hall sensor
uint16_t combinations[] = {
  0b101001010110, 0b011001010110, 0b101001100110, 0b011001100110, 0b101001010101,
  0b011001010101, 0b101001100101, 0b011001100101, 0b101010010110, 0b011010010110,
  0b101010100110, 0b011010100110, 0b101010010101, 0b011010010101, 0b101010100101,
  0b011010100101, 0b101001011010, 0b011001011010, 0b101001101010, 0b011001101010,
  0b101001011001, 0b011001011001, 0b101001101001, 0b011001101001, 0b101010011010,
  0b011010011010, 0b101010101010, 0b011010101010, 0b101010011001, 0b011010011001,
  0b101010101001, 0b011010101001, 0b100101010110, 0b010101010110, 0b100101100110,
  0b010101100110, 0b100101010101, 0b010101010101, 0b100101100101, 0b010101100101,
  0b100110010110, 0b010110010110, 0b100110100110, 0b010110100110, 0b100110010101,
  0b010110010101, 0b100110100101, 0b010110100101, 0b100101011010, 0b010101011010,
  0b100101101010, 0b010101101010, 0b100101011001, 0b010101011001, 0b100101101001,
  0b010101101001, 0b100110011010, 0b010110011010, 0b100110101010, 0b010110101010,
  0b100110011001, 0b010110011001, 0b100110101001, 0b010110101001
};

//Corresponding code for each polarity combination
String combinationNames[] = {
  "Correct",  "A1inv",  "A2inv",  "Ainv", "B1inv",
  "A1B1inv",  "A2B1inv",  "AB1inv", "B2inv",  "A1B2inv",
  "A2B2inv",  "AB2inv", "Binv", "A1Binv", "A2Binv",
  "ABinv",  "C1inv",  "A1C1inv",  "A2C1inv",  "AC1inv",
  "B1C1inv",  "A1B1C1inv",  "A2B1C1inv",  "AB1C1inv", "B2C1inv",
  "A1B2C1inv",  "A2B2C1inv",  "AB2C1inv", "BC1inv", "A1BC1inv",
  "A2BC1inv", "ABC1inv",  "C2inv",  "A1C2inv",  "A2C2inv",
  "AC2inv", "B1C2inv",  "A1B1C2inv",  "A2B1C2inv",  "AB1C2inv",
  "B2C2inv",  "A1B2C2inv",  "A2B2C2inv",  "AB2C2inv", "BC2inv",
  "A1BC2inv", "A2BC2inv", "ABC2inv",  "Cinv", "A1Cinv",
  "A2Cinv", "ACinv",  "B1Cinv", "A1B1Cinv", "A2B1Cinv",
  "AB1Cinv",  "B2Cinv", "A1B2Cinv", "A2B2Cinv", "AB2Cinv",
  "BCinv",  "A1BCinv",  "A2BCinv",  "ABCinv"
};

byte upArrow[] = {
  0b00000,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};

byte downArrow[] = {
  0b00000,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};

byte cross[] = {
  0b00000,
  0b10001,
  0b01010,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000
};

byte tick[] = {
  0b00000,
  0b00000,
  0b00000,
  0b00001,
  0b00010,
  0b10100,
  0b01000,
  0b00000
};
/*******CONSTANTS*******/

/******Initialisation functions*******/
void initRelays(void){
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void initLCD(void){
  lcd.init();                     
  lcd.backlight();
  lcd.print("State:no measurement"); 
  lcd.print("Awaiting activation");
  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);
  lcd.createChar(2, cross);
  lcd.createChar(3, tick);
}

void initHallSensors(void) {
  for (int i = FIRST_HALL_SENSOR_PIN; i <= 44 /*NB_HALL_SENSORS + FIRST_HALL_SENSOR_PIN*/; i+=2) { //MSB first
 #ifdef DEBUG
    Serial.println(i);
 #endif
    pinMode(i, INPUT_PULLUP);
  }
}

void initLEDs(void){
  LEDstrip.begin();
  LEDstrip.setBrightness(BRIGHTNESS);
  LEDstrip.fill(LEDstrip.Color(255, 255, 255)); //Yellow
  LEDstrip.show();
}

void initBuzzer(void){
  pinMode(BUZZER_PIN, OUTPUT);
}

void initSD(void){
  if (!SD.begin(CS_SD)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  File logFile = SD.open("Motor_eval.csv", FILE_WRITE);
  if (logFile){  
      String header = "No., Time, measurement, compared with correct, winding configuration"; //These will be the headers for your excel file, CHANGE "" to whatevr headers you would like to use
      logFile.println(header);
      logFile.close();
  }
}
/******Initialisation functions*******/

/******Loop functions******/
void relaysOn(void){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
}

void relaysOff(void){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

uint16_t readHallSensors(void) {
  uint16_t reading = 0;

  for (int i = FIRST_HALL_SENSOR_PIN, j = NB_HALL_SENSORS - 1 ; i <= 44; i+=2, j--) { //MSB first
    reading |= (digitalRead(i) << j); // each reading is a single bit, the bit shifting enables this
  }
#ifdef DEBUG
  Serial.print("reading = ");
  Serial.println(reading, BIN);
#endif
  return reading;
}

boolean evaluateMotor(uint16_t reading, char* s) {
  bool found = false; //true if the reading corresponds to a known configuration

  for (int i = 0; i < sizeof(combinations) / 2; i++) {
#ifdef DEBUG
    Serial.print(reading, BIN);
    Serial.print(" =? ");
    Serial.println((combinations[i]), BIN);
#endif
    if (reading == combinations[i]) {
      strcpy(s, combinationNames[i].c_str());
#ifdef DEBUG
      Serial.println(s);
#endif
      return true;
    }
  }

  strcpy(s, "Not Found"); // only arrive here if no known configuration is found
#ifdef DEBUG
  Serial.println(s);
#endif
  return false;
}

void displayNewReadingLCD(boolean comboFound, uint16_t reading, char* combinationName) {
  lcd.clear();
  lcd.home();
   lcd.print("State: ");
  lcd.print(combinationName);

  lcd.setCursor(0, 1);
  if(comboFound){
    lcd.print("Poles: ");
    for(int i=NB_HALL_SENSORS-1; i >= 0; i--)
      getBit(reading, i) ? lcd.write(0) : lcd.write(1);

    lcd.setCursor(7, 2); //align with polarities
    reading ^= combinations[CORRECT];
    for(int i=NB_HALL_SENSORS-1; i >= 0; i--){
      getBit(reading, i) ? lcd.write(2) : lcd.write(3); //Negated because with xor 1 means the values are different
    }
  }
  else{
    //lcd.print("- Please try again -");
  }
  //if true
  //1. GOOD OR BAD
  //2. COMBONAME
  //3. RAW POLE READS
  //4. WHICH POLES TO SWITCH (Using arrows?)
  //else
  //1. big X flashing
  //2. "No corresponding combination found"
}

void displayNewReadingLED(boolean comboFound, uint16_t reading){
  //comboFound=true; //Just for testing
  reading ^= combinations[0]; //Find incorrect windings: 1 where the values are different ie. wrong
  LEDstrip.clear();
  if(comboFound){
    for(int i=NB_HALL_SENSORS-1; i >= 0;i--){ //Start from MSb -> LSb
      uint32_t colour = LEDstrip.Color(0, 255, 0); //Green
      
      if(getBit(reading, i)) //This checks if the bit at the given index is 1 or 0
        colour = LEDstrip.Color(255, 0, 0); //Red
        
      LEDstrip.setPixelColor(i, colour);
      LEDstrip.show();
      delay(35);
    }
  }
  else{
    LEDstrip.fill(LEDstrip.Color(0, 0, 255)); //Blue
    LEDstrip.show();
  }
}

void displaySameReadingLCD(void){
    lcd.setCursor(0, 3);
    lcd.print("Same as previous");
    delay(500);
    lcd.setCursor(0, 3);
    lcd.print("                    "); //clear line
    delay(500);
    lcd.setCursor(0, 3);
    lcd.print("Same as previous");
}

void displaySameReadingLED(void){
  for(int j=0; j<5; j++){
    for(int i=0; i<BRIGHTNESS; i++){ //DIM LEDS
      LEDstrip.setBrightness(BRIGHTNESS-i);
      LEDstrip.show();
      delay(8);
    }
    
    delay(8);
    
    for(int i=1; i<=BRIGHTNESS;i++){ //BRIGHTEN LEDs
      LEDstrip.setBrightness(i);
      LEDstrip.show();
      delay(8);
    }
  }
}

void buzzerCorrect(void){
  int melodyOn[] = {NOTE_C5, NOTE_E5, NOTE_G5};
  int durationOn = 200;
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    tone(BUZZER_PIN, melodyOn[thisNote], durationOn);
    delay(200);
  }
}
void buzzerIncorrect(void){
  ////Play 'OFF' Sound
  int melodyOff[] = {NOTE_CS4, NOTE_C5};
  int durationOff = 200;
  for(int j = 0; j < 3; j++){
    for (int thisNote = 0; thisNote < 2; thisNote++) {
      tone(BUZZER_PIN, melodyOff[thisNote], durationOff);
      delay(20);
    }
  }
}

void writeToSD(unsigned long t, boolean comboFound, uint16_t reading, char* combinationName){
  File logFile = SD.open("Motor_eval.csv", FILE_WRITE);
  if (logFile){
      if(!comboFound){
          combinationName = "Not found";
          reading = -1;
      }
      logFile.println(String(nbMeasurements) + ","
                      + String(t) + ","
                      + String(reading) + ","
                      + String(~(reading ^= combinations[CORRECT])) + ","
                      + combinationName);
      logFile.close();
      //For debugging purposes
      String s = (String(nbMeasurements) + ","
                      + String(t) + ","
                      + String(reading) + ","
                      + String(~(reading ^= combinations[CORRECT])) + ","
                      + combinationName);
      Serial.println(s);
    }
}
/******Loop functions******/

/******Supplementary function******/
boolean getBit(uint16_t b, uint8_t n){
  uint16_t mask = 1<<n;
  return b & mask;
}
/******Supplementary function******/
