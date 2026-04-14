//**************************************************
// Enums
//**************************************************

typedef enum {
  MOTORS_CONTROL_ID_LEFT = 0,//
  MOTORS_CONTROL_ID_RIGHT,
  _MOTORS_CONTROL_ID_MAX,
} motors_control_id_t;

//**************************************************
// Function Prototypes
//**************************************************

void motors_control_init();

void motors_control_set_speed(const motors_control_id_t id, const int speed);

void motors_control_logic();