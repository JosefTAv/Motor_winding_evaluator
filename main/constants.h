#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CORRECT 0

#define UPARROW 0
#define DOWNARROW 1
#define CROSS 2
#define TICK 3

#define ENABLE_PIN 19
#define IN2 12 // tiene que cerrar que hay error
#define IN1 13 // tiene que abrir cuando ha terminado de medir (circula el corriente)

#define FIRST_HALL_SENSOR_PIN 22
#define NB_HALL_SENSORS 12
#define LAST_HALL_SENSOR_PIN 44

#define LED_PIN 10
#define NB_LEDS 12
#define BRIGHTNESS 20

#define BUZZER_PIN 9

#define CS_SD 53
#define FILENAME "log" //.csv file
#define HEADER "No.,Time,Correct(BIN),Measurement(BIN),Correct poles(BIN),Winding type"
#endif 

//Interruptor abajo (a HIGH) -> abre IN2 
// Memoria el 20 Lunes
