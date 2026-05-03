/*
void reset_turn() {
  motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, 0);
  motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, 0);
}

bool turn() {
  // --- CONFIGURAÇÕES ---
  const float FRONT_THRESHOLD = 40.0; // Distância para considerar "caminho livre" na frente
  const float SIDE_TARGET = 15.0;    // Distância ideal da nova parede lateral após o giro
  const int TURN_BASE_SPEED = 450;   // Velocidade de rotação

  distance_t distance = distance_sensors_get();

  // 1. CONDIÇÃO DE TÉRMINO:
  // O sensor frontal não vê mais nada perto E o lateral já detectou a parede
  if (distance.front > FRONT_THRESHOLD && distance.left < 25.0) {
    reset_turn();

    motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, 0);
    motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, 0);
    
    return true; // Giro concluído
  }

  // 2. LÓGICA DE MOVIMENTO (Giro sobre o eixo):
  // Enquanto não termina, ele aplica um diferencial para girar.
  // Se o frontal ainda está muito perto, ele gira mais rápido.
  
  float error_front = distance.front; 
  int speed_adj = 0;

  // Se o sensor frontal ainda vê a parede, mantemos o giro forte
  if (distance.front < FRONT_THRESHOLD) {
    speed_adj = 100; // Torque extra enquanto a parede frontal está bloqueando
  }

  // Rodas em sentidos opostos para girar no lugar
  int left_pwm  = (TURN_BASE_SPEED + speed_adj);
  int right_pwm =  -(TURN_BASE_SPEED + speed_adj);

  motors_control_set_speed(MOTORS_CONTROL_ID_LEFT, left_pwm);
  motors_control_set_speed(MOTORS_CONTROL_ID_RIGHT, right_pwm);

  return false; // Ainda girando
}
*/