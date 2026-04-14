#include <sys/_stdint.h>
#include "motors_control.hpp"
#include <Pico4Drive.h>

//**************************************************
// Defines
//**************************************************

#define PWM_MAX     200
#define DEAD_ZONE_P 30
#define DEAD_ZONE_N 20

#define FF  2
#define P   2
#define I   2

//**************************************************
// Types
//**************************************************

struct motor_params_t{
  uint8_t encoder_a_pin;
  uint8_t encoder_b_pin;
  driver_num_t driver;
  int encoder_position; 
  int last_encoder_position;
  int8_t encoder_state;
  int speed_ref;
  int int_error;
};

//**************************************************
// Function Prototypes
//**************************************************

static void set_voltage(driver_num_t driver, int v1);

//**************************************************
// Globals
//**************************************************

static pico4drive_t s_pico4drive;

static const int8_t s_encoder_table[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};

static motor_params_t s_motors_params[_MOTORS_CONTROL_ID_MAX] = {
  { // MOTORS_CONTROL_ID_LEFT
    .encoder_a_pin=0,.encoder_b_pin=1,
    .driver=p2d_drv4,
    .encoder_position=0, .last_encoder_position=0,
    .speed_ref=0,
    .int_error=0,
  },
  { // MOTORS_CONTROL_ID_RIGHT
    .encoder_a_pin=2,.encoder_b_pin=3,
    .driver=p2d_drv3,
    .encoder_position=0, .last_encoder_position=0,
    .speed_ref=0,
    .int_error=0,
  }
};

static long s_last_millis_speed_control = 0;

//**************************************************
// Public Funtions
//**************************************************

void motors_control_init(){
  s_pico4drive.init();

  for(int i = 0; i < _MOTORS_CONTROL_ID_MAX; i++){
    pinMode(s_motors_params[i].encoder_a_pin, INPUT_PULLUP);
    pinMode(s_motors_params[i].encoder_b_pin, INPUT_PULLUP);
    s_pico4drive.set_driver_PWM(0, s_motors_params[i].driver);
  }
}

void motors_control_logic(){
  long current_millis = millis();  
  
  // Read Encoder
  for(int i = 0; i < _MOTORS_CONTROL_ID_MAX; i++){
    byte next_state, table_input;

    next_state = digitalRead(s_motors_params[i].encoder_a_pin) << 1;
    next_state |= digitalRead(s_motors_params[i].encoder_b_pin);

    table_input = (s_motors_params[i].encoder_state << 2) | next_state;
    s_motors_params[i].encoder_position += s_encoder_table[table_input];
    s_motors_params[i].encoder_state = next_state;
  }

  // Speed Control
  if (current_millis - s_last_millis_speed_control >= 20){
    s_last_millis_speed_control = current_millis;

    for(int i = 0; i < _MOTORS_CONTROL_ID_MAX; i++){
      int encoder_speed = s_motors_params[i].encoder_position - s_motors_params[i].last_encoder_position;  
      int error = s_motors_params[i].speed_ref - encoder_speed;
      s_motors_params[i].int_error = s_motors_params[i].int_error + error;

      if(s_motors_params[i].int_error > 200){
        s_motors_params[i].int_error = 200;
      } 

      int ctrl = FF * s_motors_params[i].speed_ref + P * error + I * s_motors_params[i].int_error;
      set_voltage(s_motors_params[i].driver, ctrl);

      s_motors_params[i].last_encoder_position = s_motors_params[i].encoder_position;
    }
  }
}

void motors_control_set_speed(const motors_control_id_t id,const int speed){
  s_motors_params[id].speed_ref = speed;
}

//**************************************************
// Static Funtions
//**************************************************

static void set_voltage(driver_num_t driver, int v1)
{ 
  if (v1 > 0)
  {
    v1 = v1 + DEAD_ZONE_P;
    
    if (v1 > PWM_MAX){
      v1 = PWM_MAX;
    }

    s_pico4drive.set_driver_PWM(v1, driver);
    return;
  }

  if (v1 < 0){
    v1 = -v1;
    v1 = v1 + DEAD_ZONE_N;

    if (v1 > PWM_MAX){
      v1 = PWM_MAX;
    }

    s_pico4drive.set_driver_PWM(-v1, driver);
    return;
  }

  s_pico4drive.set_driver_PWM(0, driver);
}