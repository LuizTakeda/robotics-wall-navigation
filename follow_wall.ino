/*
// Configurações de Movimentação
#define SPEED_BOTTOM 400
#define SPEED_TOP 700
#define SPEED_MAX  1024

#define TURN_SPEED 500

// Variáveis PID
double Setpoint = 20.0; // Distância da parede em cm
double Input, Output;
double Kp = 2.0, Ki = 0.3, Kd = 10.0; // Kp maior para resposta rápida, Ki baixo para evitar oscilação

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void reset_follow_wall(){
  myPID.SetOutputLimits(-300, 300); 
  myPID.SetSampleTime(50);

  // Reset
  myPID.SetMode(MANUAL);
  Output = 0;               
  myPID.SetMode(AUTOMATIC);
}

bool follow_wall() {
  distance_t distance = distance_sensors_get();

  if(distance.front <= 20.0){
    reset_follow_wall();
    
    motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, 0);
    motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, 0);

    return true;
  }
  
  Input = distance.left;
  double error = Setpoint - Input;
  double abs_error = abs(error);

  // --- 2. CÁLCULO DO PID ---
  myPID.Compute();

  // --- 3. VELOCIDADE BASE SUAVE ---
  // Diminuí o fator de 60 para 30 para a redução ser menos "no soco"
  int dynamic_base = SPEED_TOP - (int)(abs_error * 30);
  dynamic_base = constrain(dynamic_base, SPEED_BOTTOM, SPEED_TOP);

  // --- 4. MIXER DE MOTORES ---
  int left_pwm  = dynamic_base + (int)Output;
  int right_pwm = dynamic_base - (int)Output;

  left_pwm = constrain(left_pwm, -SPEED_MAX, SPEED_MAX);
  right_pwm = constrain(right_pwm, -SPEED_MAX, SPEED_MAX);

  motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, left_pwm);
  motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, right_pwm);

  // Debug mais limpo para o Serial Plotter
  static uint32_t last_print = 0;
  if(millis() - last_print > 50){
    Serial.printf("In:%.1f Out:%.1f Base:%d L:%d R:%d\n", Input, Output, dynamic_base, left_pwm, right_pwm);
    last_print = millis();
  }

  return false;
}
*/