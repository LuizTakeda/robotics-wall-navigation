#include <Arduino.h>
#include "distance_sensors.hpp"
#include "motors_control.hpp"

#define V 0.2

#define BASE 300

#define FRONT_GAIN 10
#define FRONT_DISTANCE_REF 20.0

#define LEFT_GAIN 10
#define LEFT_DISTANCE_REF 15.0

void setup()
{
  Serial.begin(115200);
  
  motors_control_init();
  distance_sensors_init();
}

long last_millis = 0;

void loop(){
  if(last_millis - millis() > 60){
    last_millis = millis();
    // Observation
    auto distance = distance_sensors_get();
    
    if(distance.front > FRONT_DISTANCE_REF){
      auto left_erro = LEFT_DISTANCE_REF - distance.left;
      auto left_w = LEFT_GAIN * left_erro;

      int left_pwm = BASE + left_w;
      int right_pwm = BASE - left_w;

      Serial.printf("lv:%d rv:%d\n",left_pwm, right_pwm);

      // Action
      motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, left_pwm);
      motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, right_pwm);
    }else{
      auto front_erro = FRONT_DISTANCE_REF - distance.front;
      auto front_w = FRONT_GAIN * front_erro;

      int left_pwm = BASE  + front_w;
      int right_pwm = BASE - front_w;

      //Serial.printf("df:%.2f dl:%.2f lv:%d rv:%d\n",distance.front, distance.left ,left_pwm, right_pwm);

      Serial.printf("lv:%d rv:%d\n",left_pwm, right_pwm);
      // Action
      motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, left_pwm);
      motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, right_pwm);
    }

  }

  // Logic
  //motors_control_logic();
  distance_sensors_logic();
}