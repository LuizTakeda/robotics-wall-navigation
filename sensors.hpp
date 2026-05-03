#pragma once

struct sensors_t{
  float front_distance, left_distance;
  float angle;
};

namespace Sensors{
  void initialize();

  void logic();
  
  sensors_t get();
}