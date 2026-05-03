#include <Arduino.h>
#include <PID_v1.h>
#include "motors_control.hpp"
#include "sensors.hpp"

//**************************************************
// Typedefs
//**************************************************

enum state_t {
  STATE_INITIALIZE,
  STATE_FOLLOW_WALL_1,
  STATE_TURN_1,
  STATE_FOLLOW_WALL_2,
  STATE_TURN_2,
  STATE_FOLLOW_WALL_3,
  STATE_APPROACH_WALL,
  STATE_STOP
};

//**************************************************
// Globals
//**************************************************

static state_t s_current_state = STATE_INITIALIZE;

//**************************************************
// Public Functions
//**************************************************

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Sensors::initialize();
}


void loop() {
  Sensors::logic();  
}