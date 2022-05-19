#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CORRECT 0
#define ENABLE_PIN 19//46
#define FIRST_HALL_SENSOR_PIN 22
#define NB_HALL_SENSORS 12
#define NB_LEDS 12
#define LED_PIN 10
#define IN2 12 // tiene que cerrar que hay error
#define IN1 13 // tiene que cerrar cuando ha terminado de medir
#define BRIGHTNESS 20

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

#endif
