//**************************************************
// Types
//**************************************************

struct distance_t{
  float front;
  float left;
};

//**************************************************
// Function Prototypes
//**************************************************

void distance_sensors_init();

distance_t distance_sensors_get();