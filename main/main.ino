#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include "constants.h"

//using std::vector;
//#define DEBUG

#define id(x) #x //return name of variable

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

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_NeoPixel LEDstrip(NB_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800); //12 = NB_HALL_SENSORS

volatile boolean activated = false;

//Function prototypes
void initLCD();
void initHallSensors(void);
void initLEDs(void);
uint16_t readHallSensors(void);
boolean evaluateMotor(uint16_t reading, String &s);
void displayNewReadingLCD(boolean comboFound, uint16_t reading, String combinationName);
void displayNewReadingLED(boolean comboFound, uint16_t reading);
void displaySameReadingLCD(void);
void displaySameReadingLED(void);
void enableSignalISR(void);
inline bool getBit() __attribute__((always_inline)); //inline in order to improve speed

void setup() {
  Serial.begin(115200);
  delay(50);

  //initialise IO peripherals
  initLCD();
  initHallSensors();
  initLEDs();
  pinMode(ENABLE_PIN, INPUT_PULLUP);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENABLE_PIN), enableSignalISR, FALLING); //interrupt for the enable signal
}

uint16_t oldReading = 0;
int k=0;

void loop() {
  /*if(digitalRead(ENABLE_PIN) == 1){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
  }
  else{    
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }*/
  if (activated) {
    delay(2000);
    //uint16_t reading = combinations[k]; //test value for use without the hall sensors
    uint16_t reading = readHallSensors();
    
    if(reading!=oldReading){
      oldReading=reading;
      String combinationName = "";
      boolean comboFound = evaluateMotor(reading, combinationName);
      comboFound = true; //for testing
      displayNewReadingLCD(comboFound, reading, combinationName);
      displayNewReadingLED(comboFound, reading);
      //Serial.println("----");
    }
    else{
      displaySameReadingLCD();
      displaySameReadingLED();
    }
    
    activated = false;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    k++;
  }
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
    Serial.println(i);
    pinMode(i, INPUT_PULLUP);
  }
}

void initLEDs(void){
  LEDstrip.begin();
  LEDstrip.setBrightness(BRIGHTNESS);
  LEDstrip.fill(LEDstrip.Color(255, 255, 255)); //Yellow
  LEDstrip.show(); // Initialize all pixels to 'off'
}

void enableSignalISR(void) {
  activated = true;
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
}

uint16_t readHallSensors(void) {
  uint16_t reading = 0;

  for (int i = FIRST_HALL_SENSOR_PIN, j = NB_HALL_SENSORS - 1 ; i <= 44; i+=2, j--) { //MSB first
    reading |= (digitalRead(i) << j); // each reading is a single bit, the bit shifting enables this
  }
//#ifdef DEBUG
  Serial.print("reading = ");
  Serial.println(reading, BIN);
//#endif
  return reading;
}

boolean evaluateMotor(uint16_t reading, String &s) {
  bool found = false; //true if the reading corresponds to a known configuration

  for (int i = 0; i < sizeof(combinations) / 2; i++) {
#ifdef DEBUG
    Serial.print(reading, BIN);
    Serial.print("==");
    Serial.println((combinations[i]), BIN);
#endif
    if (reading == combinations[i]) {
      s = combinationNames[i];
#ifdef DEBUG
      Serial.println(s);
#endif
      return true;
    }
  }

  s = "Not found"; // only arrive here if no known configuration is found
#ifdef DEBUG
  Serial.println(s);
#endif
  return false;
}

void displayNewReadingLCD(boolean comboFound, uint16_t reading, String combinationName) {
  lcd.clear();
  lcd.home();
  lcd.print("State: " + combinationName);
  lcd.setCursor(0, 1);
  if(comboFound){
    lcd.print("Poles: ");
    for(int i=0; i < NB_HALL_SENSORS; i++)
      getBit(reading, i) ? lcd.write(0) : lcd.write(1);

    lcd.setCursor(7, 2);
    reading ^= combinations[CORRECT];
    for(int i=0; i < NB_HALL_SENSORS; i++){
      getBit(reading, i) ? lcd.write(2) : lcd.write(3); //Negated because with xor 1 means the values are different
    }
  }
  else{
    lcd.print("- Please try again -");
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
    lcd.print("                    ");
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

boolean getBit(uint16_t b, uint8_t n){
  uint16_t mask = 1<<n;
  return b & mask;
}
