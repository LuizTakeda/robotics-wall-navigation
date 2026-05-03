#include "sensors.hpp"
#include <Arduino.h>
#include <math.h>
#include "Wire.h"
#include "Adafruit_VL53L0X.h"
#include "Adafruit_MPU6050.h"

//**************************************************
// Defines
//**************************************************

#define DISTANCE_FILTER_ALPHA 0.3

//**************************************************
// Globals
//**************************************************

static Adafruit_VL53L0X s_front_tof = Adafruit_VL53L0X();
static Adafruit_VL53L0X s_left_tof = Adafruit_VL53L0X();
static Adafruit_MPU6050 s_mpu = Adafruit_MPU6050();

static float s_front_distance, s_left_distance;
static float s_angle = 0;
static float s_gyro_offset = 0;

//**************************************************
// Function Prototypes 
//**************************************************

static void initilize_i2c();
static void initilize_tofs();
static void initilize_gryo();
static void calibrate_gryo();
static float apply_ema_filter(float current, float previous, float alpha);
static void update_distance(Adafruit_VL53L0X *sensor, float *dist, float *prev_dist, bool *is_first);
static void update_angle();

//**************************************************
// Namespace Functions
//**************************************************

void Sensors::initialize(){
  initilize_i2c();
  initilize_tofs();
  initilize_gryo();
}

void Sensors::logic(){
  static uint32_t last_millis = millis();
  static float previous_front_distance = 0;
  static float previous_left_distance = 0;
  static bool is_first_front_distance = true;
  static bool is_first_left_distance = true;

  uint32_t current_time = millis();

  if (current_time - last_millis >= 20) {
    last_millis = current_time;

    update_distance(&s_front_tof, &s_front_distance, &previous_front_distance, &is_first_front_distance);
    update_distance(&s_left_tof, &s_left_distance, &previous_left_distance, &is_first_left_distance);
    
    update_angle();

    Serial.printf("DF: %.2f DL: %.2f | An: %.2f\n", s_front_distance, s_left_distance, s_angle);
  }
}

sensors_t Sensors::get(){
  sensors_t sensors = {
    .front_distance = s_front_distance,
    .left_distance = s_left_distance,
    .angle = s_angle
  };

  return sensors;
}

//**************************************************
// Static Functions
//**************************************************

static void initilize_i2c(){
  // I2C0 - Pins GP4 and GP5
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin(); 
  
  // I2C1 - Pins GP6 and GP7
  Wire1.setSDA(6);
  Wire1.setSCL(7);
  Wire1.begin();
}

static void initilize_tofs(){
  if (!s_front_tof.begin(0x29, false, &Wire)) {
    Serial.println("Error: front_tof (I2C0)");
  } 
  else {
    s_front_tof.setMeasurementTimingBudgetMicroSeconds(20000);
    s_front_tof.startRangeContinuous();
  }

  if (!s_left_tof.begin(0x29, false, &Wire1)) {
    Serial.println("Error: left_tof (I2C1)");
  } 
  else {
    s_left_tof.setMeasurementTimingBudgetMicroSeconds(20000);
    s_left_tof.startRangeContinuous();
  }
}

static void initilize_gryo(){
  if (!s_mpu.begin(0x68, &Wire)) {
    Serial.println("Error: MPU6050 (I2C1)");
    return;
  }

  calibrate_gryo();
}

static void calibrate_gryo(){
  Serial.println("Calibrando Giroscópio... Mantenha o robô parado.");
  float sum_x = 0;
  const int samples = 200; 

  for(int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    s_mpu.getEvent(&a, &g, &temp);
    sum_x += g.gyro.x;
    delay(5); 
  }

  s_gyro_offset = sum_x / (float)samples;
  Serial.print("Offset X calculado: "); Serial.println(s_gyro_offset);
}

static float apply_ema_filter(float current, float previous, float alpha) {
  return (alpha * current) + ((1.0f - alpha) * previous);
}

static void update_distance(Adafruit_VL53L0X *sensor, float *dist, float *prev_dist, bool *is_first) {
  if (!sensor->isRangeComplete()) return;

  float raw_dist = (float)sensor->readRange() * 0.1f; 

  if (*is_first) {
    *prev_dist = raw_dist;
    *is_first = false;
  }

  *prev_dist = apply_ema_filter(raw_dist, *prev_dist, DISTANCE_FILTER_ALPHA);
  *dist = *prev_dist;
}

static void update_angle(){
  static uint32_t last_angle_micros = 0;

  uint32_t current_micros = micros();
  
  if (last_angle_micros == 0) {
    last_angle_micros = current_micros;
    return;
  }

  float dt = (current_micros - last_angle_micros) / 1000000.0f;
  last_angle_micros = current_micros;

  sensors_event_t a, g, temp;
  s_mpu.getEvent(&a, &g, &temp);

  // Removendo bias e convertendo para graus
  float gyro_corrected = g.gyro.x - s_gyro_offset; 
  float gyro_deg = gyro_corrected * (180.0f / M_PI);

  // Deadzone dinâmica: ajuda a reduzir o drift quando parado
  if (abs(gyro_deg) > 0.20f) {
    s_angle += gyro_deg * dt;
  }

  if (s_angle > 180) s_angle -= 360;
  else if (s_angle < -180) s_angle += 360;
}
