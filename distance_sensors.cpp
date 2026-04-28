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
static void left_echo_interrupt();
static void front_echo_interrupt();
static void triggers_echo(int trig_pin, int echo_pin);


//**************************************************
// Globals
//**************************************************

volatile long left_tempo_inicio = 0;
volatile long left_duracao_pulso = 0;
volatile bool left_novo_dado = false;

volatile long front_tempo_inicio = 0;
volatile long front_duracao_pulso = 0;
volatile bool front_novo_dado = false;

volatile float left_distance = 0;
volatile float front_distance = 0;

//**************************************************
// Public Functions
//**************************************************

void distance_sensors_init(){
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);

  pinMode(LEFT_TRIG_PIN, OUTPUT);
  pinMode(LEFT_ECHO_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ECHO_PIN), left_echo_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FRONT_ECHO_PIN), front_echo_interrupt, CHANGE);
}

void distance_sensors_logic(){
  static unsigned long ultima_leitura = 0;

  if (millis() - ultima_leitura >= 50) {
    triggers_echo(FRONT_TRIG_PIN, FRONT_ECHO_PIN);
  }

  if(millis() - ultima_leitura >= 100){
    triggers_echo(LEFT_TRIG_PIN, LEFT_ECHO_PIN);
    ultima_leitura = millis();
  }

  if(left_novo_dado){
    left_distance = left_duracao_pulso / 58.2;
    
    left_novo_dado = false;
  }

  if(front_novo_dado){
    front_distance= front_duracao_pulso / 58.2;
    
    front_novo_dado = false;
  }

  //Serial.printf("fd:%f ld:%f\n", front_distance, left_distance);
}

distance_t distance_sensors_get(){
  distance_t distance = {
    .front=front_distance,
    .left=left_distance
  };

  return distance;
}

//**************************************************
// Static Functions
//**************************************************

static void left_echo_interrupt(){
  if (digitalRead(LEFT_ECHO_PIN) == HIGH) {
    left_tempo_inicio = micros();
    return;
  }

  left_duracao_pulso = micros() - left_tempo_inicio;
  left_novo_dado = true;
}

static void front_echo_interrupt(){
  if (digitalRead(FRONT_ECHO_PIN) == HIGH) {
    front_tempo_inicio = micros();
    return;
  }
  
  front_duracao_pulso = micros() - front_tempo_inicio;
  front_novo_dado = true;
}

static void triggers_echo(int trig_pin, int echo_pin){
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
}