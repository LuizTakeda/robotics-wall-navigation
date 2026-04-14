#include <Arduino.h>
#include "distance_sensors.hpp"
#include "motors_control.hpp"

void setup()
{
  Serial.begin(115200);
  
  motors_control_init();
  distance_sensors_init();
}

void loop(){
  motors_control_logic();

  if (Serial.available() > 0){
    int ref = Serial.parseInt();
    
    motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, ref);
    motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, ref);
    
    while (Serial.available()){
      Serial.read();
    }
  }
}