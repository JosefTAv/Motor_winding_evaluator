#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>

//using std::vector;
//#define DEBUG

#define id(x) #x //return name of variable
#define FIRST_HALL_SENSOR_PIN 22
#define NB_HALL_SENSORS 12
#define LED_PIN 13
#define BRIGHTNESS 20

#define Correct 1
#define A1inv 2
#define A2inv 3
#define Ainv 4
#define B1inv 5
#define A1B1inv 6
#define A2B1inv 7
#define AB1inv 8
#define B2inv 9
#define A1B2inv 10
#define A2B2inv 11
#define AB2inv 12
#define Binv 13
#define A1Binv 14
#define A2Binv 15
#define ABinv 16
#define C1inv 17
#define A1C1inv 18
#define A2C1inv 19
#define AC1inv 20
#define B1C1inv 21
#define A1B1C1inv 22
#define A2B1C1inv 23
#define AB1C1inv 24
#define B2C1inv 25
#define A1B2C1inv 26
#define A2B2C1inv 27
#define AB2C1inv 28
#define BC1inv 29
#define A1BC1inv 30
#define A2BC1inv 31
#define ABC1inv 32
#define C2inv 33
#define A1C2inv 34
#define A2C2inv 35
#define AC2inv 36
#define B1C2inv 37
#define A1B1C2inv 38
#define A2B1C2inv 39
#define AB1C2inv 40
#define B2C2inv 41
#define A1B2C2inv 42
#define A2B2C2inv 43
#define AB2C2inv 44
#define BC2inv 45
#define A1BC2inv 46
#define A2BC2inv 47
#define ABC2inv 48
#define Cinv 49
#define A1Cinv 50
#define A2Cinv 51
#define ACinv 52
#define B1Cinv 53
#define A1B1Cinv 54
#define A2B1Cinv 55
#define AB1Cinv 56
#define B2Cinv 57
#define A1B2Cinv 58
#define A2B2Cinv 59
#define AB2Cinv 60
#define BCinv 61
#define A1BCinv 62
#define A2BCinv 63
#define ABCinv 64

//Types of known coil winding combinations
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

//Known coil winding combinations
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

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_NeoPixel LEDstrip = Adafruit_NeoPixel(12, LED_PIN, NEO_GRB + NEO_KHZ800); //12 = NB_HALL_SENSORS

const int enablePin = 3;
volatile boolean activated = false;
uint32_t i = 21;

//Function prototypes
void initHallSensors(void);
void initLEDs(void);
uint16_t readHallSensors(void);
boolean evaluateMotor(uint16_t reading, String &s);
void displayLCD(boolean comboFound, String combinationName);
void displayNewReadingLED(boolean comboFound, uint16_t reading);
void enableSignalISR(void);

void setup() {
  Serial.begin(115200);
  delay(50);
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  pinMode(enablePin, INPUT_PULLUP);
  initHallSensors();
  initLEDs();
  attachInterrupt(digitalPinToInterrupt(enablePin), enableSignalISR, FALLING); //interrupt for the enable signal
}

uint16_t oldReading = 0;

void loop() {
  
  if (i % 10000000 == 0){ //simulate enable signal
    activated = true;
    Serial.println(i%10);
  }
    
  if (activated) {
    //uint16_t reading = 0b010110101000; //test value for use without the hall sensors
    uint16_t reading = readHallSensors();
    if(reading!=oldReading){
      oldReading=reading;
      String combinationName = "";
      boolean comboFound = evaluateMotor(reading, combinationName);
      displayNewReadingLED(comboFound, reading);
      //Serial.println("----");
    }
    else{
      displaySameReadingLED();
    }
    activated = false;
  }
}

void initHallSensors(void) {
  for (int i = FIRST_HALL_SENSOR_PIN; i < NB_HALL_SENSORS + FIRST_HALL_SENSOR_PIN; i++) { //MSB first
    Serial.println(i);
    pinMode(i, INPUT_PULLUP);
  }
}

void initLEDs(void){
  LEDstrip.begin();
  LEDstrip.setBrightness(BRIGHTNESS);
  LEDstrip.fill(LEDstrip.Color(255, 233, 0));
  LEDstrip.show(); // Initialize all pixels to 'off'
}

void enableSignalISR(void) {
  activated = true;
  Serial.println("ON");
}

uint16_t readHallSensors(void) {
  uint16_t reading = 0;

  for (int i = FIRST_HALL_SENSOR_PIN; i < NB_HALL_SENSORS + FIRST_HALL_SENSOR_PIN; i++) { //MSB first
    reading |= (digitalRead(i) << (NB_HALL_SENSORS + FIRST_HALL_SENSOR_PIN - 1 - i)); // each reading is a single bit, the bit shifting enables this
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

  s = "No corresponding combination found"; // only arrive here if no known configuration is found
#ifdef DEBUG
  Serial.println(s);
#endif
  return false;
}

//TODO
void displayNewReadingLCD(boolean comboFound, String combinationName) {
  //if true
  //1. GOOD OR BAD
  //2. COMBONAME
  //3. RAW POLE READS
  //4. WHICH POLES TO SWITCH (Using arrows?)
  //else
  //1. big X flashing
  //2. "No corresponding combination found"
}

//TOFINISH
void displayNewReadingLED(boolean comboFound, uint16_t reading){
  //comboFound=true; //Just for testing
  reading ^= combinations[0]; //Find incorrect windings
  LEDstrip.clear();
  if(comboFound){
    for(int i=NB_HALL_SENSORS-1; i >= 0;i--){ //Start from MSb -> LSb
      uint32_t colour = LEDstrip.Color(0, 255, 0); //Green
      
      uint16_t mask = 1<<i;
      uint16_t temp = reading & mask; //reading &= 1<<i;
      if(temp==0) //These three lines extract the bit corresponding to the LED of a winding
        colour = LEDstrip.Color(255, 0, 0); //Red
        
      LEDstrip.setPixelColor(i, colour);
      LEDstrip.show();
      delay(35);
    }
  }
  else{
    LEDstrip.fill(LEDstrip.Color(0, 0, 255));
    LEDstrip.show();
  }
}

void displaySameReadingLED(void){
  for(int j=0; j<5; j++){
    for(int i=0; i<BRIGHTNESS; i++){ //DIM
      LEDstrip.setBrightness(BRIGHTNESS-i);
      LEDstrip.show();
      delay(10);
    }
    
    delay(10);
    
    for(int i=1; i<=BRIGHTNESS;i++){ //BRIGHTEN
      LEDstrip.setBrightness(i);
      LEDstrip.show();
      delay(10);
    }
  }
}
