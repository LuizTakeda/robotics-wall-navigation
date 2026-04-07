#include <Arduino.h>
#include <Pico4Drive.h>
#include "distance_sensors.hpp"

#define ENC_A 0
#define ENC_B 1

#define RIGHT_ENC_A 2 
#define RIGHT_ENC_B 3

#define MODEPWM             false
#define MODECTRLANGLE       false
#define MODECTRLANGLE_RAND  false
#define MODECTRLSPEED       true

#define MAXPWM 200
#define deadZoneP 30
#define deadZoneN 20

byte encoder_state;
byte right_encoder_state;

pico4drive_t pico4drive;

int encoder_table[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
volatile int encoder_pos = 0;
volatile int right_encoder_pos = 0;
volatile int last_encoder_pos = 0;
volatile int right_last_encoder_pos = 0;
int speed = 0;
int right_speed = 0;
int int_error;
int right_int_error;
int i;
int right_i;

unsigned long previousMillis = 0;
const long interval = 100;

int ref = 0;
int right_ref = 0;

void setMotorVoltage(driver_num_t driver,int v1)
{
  if (v1 == 0)
  {
    pico4drive.set_driver_PWM(0, driver);
  }
  else if (v1 > 0)
  {
    v1 = v1 + deadZoneP;
    if (v1 > MAXPWM)
      v1 = MAXPWM;
    pico4drive.set_driver_PWM(v1, driver);
  }
  else
  {
    v1 = -v1;
    v1 = v1 + deadZoneN;
    if (v1 > MAXPWM)
      v1 = MAXPWM;
    pico4drive.set_driver_PWM(-v1, driver);
  }
}

void read_encoder()
{
  byte next_state, table_input;

  next_state = digitalRead(ENC_A) << 1;
  next_state |= digitalRead(ENC_B);

  table_input = (encoder_state << 2) | next_state;
  encoder_pos += encoder_table[table_input];
  encoder_state = next_state;


  next_state = digitalRead(RIGHT_ENC_A) << 1;
  next_state |= digitalRead(RIGHT_ENC_B); 

  table_input = (right_encoder_state << 2) | next_state;
  right_encoder_pos += encoder_table[table_input];
  right_encoder_state = next_state;
}

void setup()
{
  pico4drive.init();
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  pinMode(RIGHT_ENC_A, INPUT_PULLUP);
  pinMode(RIGHT_ENC_B, INPUT_PULLUP);

  
  pico4drive.set_driver_PWM(0, p2d_drv4);
  pico4drive.set_driver_PWM(0, p2d_drv3);
  
  distance_sensors_init();

  Serial.begin(115200);
}

void loop()
{
  read_encoder();
  unsigned long currentMillis = millis();
  /*
  if (MODEPWM)
  {
    if (currentMillis - previousMillis >= 100)
    {
      previousMillis = currentMillis;
      Serial.println(encoder_pos);
    }
    if (Serial.available() > 0)
    {
      ref = Serial.parseInt();
      while (Serial.available())
        Serial.read();
      setMotorVoltage(ref);
    }
  }

  if (MODECTRLANGLE)
  {
    if (Serial.available() > 0)
    {
      ref = Serial.parseInt();
      while (Serial.available())
        Serial.read();
      int_error = 0;
    }
    if (currentMillis - previousMillis >= 50)
    {
      previousMillis = currentMillis;
      int error = ref - encoder_pos;
      int_error = int_error + error;
      if (int_error > 100)
        int_error = 100;
      if (int_error < -100)
        int_error = -100;

      int P = 5;
      int I = 1;
      int ctrl = P * error + I * int_error;
      Serial.print(error);
      Serial.print(",");
      Serial.println(encoder_pos);
      setMotorVoltage(ctrl);
    }
  }

  if (MODECTRLANGLE_RAND)
  {

    if (currentMillis - previousMillis >= 50)
    {
      previousMillis = currentMillis;
      if (i++ > 200)
      {
        i = 0;
        ref = random(-800, 800);
        int_error = 0;
      }
      int error = ref - encoder_pos;
      int_error = int_error + error;
      if (int_error > 100)
        int_error = 100;
      if (int_error < -100)
        int_error = -100;
      int P = 5;
      int I = 2;
      int ctrl = P * error + I * int_error;
      Serial.print(ref);
      Serial.print(",");
      Serial.print(ctrl);
      Serial.print(",");
      Serial.println(encoder_pos);
      setMotorVoltage(ctrl);
    }
  }
  */

  if (MODECTRLSPEED)
  {
    if (Serial.available() > 0)
    {
      ref = Serial.parseInt();
      right_ref = ref;
      while (Serial.available())
        Serial.read();
    }

    if (currentMillis - previousMillis >= 20)
    {
      previousMillis = currentMillis;

      distance_t distance = distance_sensors_get();
      
      int encoder_speed = encoder_pos - last_encoder_pos;
      int right_encoder_speed = right_encoder_pos - right_last_encoder_pos;

      int error = ref - encoder_speed;
      int right_error = right_ref - right_encoder_speed;

      int_error = int_error + error;
      right_int_error = right_int_error + right_error;

      if (int_error > 200)
        int_error = 200;

      if (right_int_error > 200)
        right_int_error = 200;

      int ff = 2;
      int P = 2;
      int I = 2;
      int ctrl = ff * ref + P * error + I * int_error;
      setMotorVoltage(p2d_drv4, ctrl);
      
      int right_ff = 2;
      int right_P = 2;
      int right_I = 2;
      int right_ctrl = right_ff * right_ref + right_P * right_error + right_I * right_int_error;
      setMotorVoltage(p2d_drv3, right_ctrl);

      Serial.printf("L:%d,%d R:%d,%d DF:%f, LF: %f\n",ctrl, encoder_speed, right_ctrl, right_encoder_speed, distance.front, distance.left);

      last_encoder_pos = encoder_pos;
      right_last_encoder_pos = right_encoder_pos;
    }
  }
}