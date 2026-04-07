#include "Arduino.h"
#include "distance_sensors.hpp"

//**************************************************
// Defines
//**************************************************

#define FRONT_TRIG_PIN 9
#define FRONT_ECHO_PIN 8

#define LEFT_TRIG_PIN 6
#define LEFT_ECHO_PIN 5

//**************************************************
// Function Prototypes
//**************************************************

static float measure_distance(int trig_pin, int echo_pin);

//**************************************************
// Public Functions
//**************************************************

void distance_sensors_init(){
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);

  pinMode(LEFT_TRIG_PIN, OUTPUT);
  pinMode(LEFT_ECHO_PIN, INPUT);
}

distance_t distance_sensors_get(){
  distance_t distance;

  distance.front = measure_distance(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
  distance.left = measure_distance(LEFT_TRIG_PIN, LEFT_ECHO_PIN);

  return distance;
}

//**************************************************
// Static Functions
//**************************************************

static float measure_distance(int trig_pin, int echo_pin){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  
  long duration = pulseIn(echo_pin, HIGH);
  return duration * 0.034 / 2;
}