#include "Primitives.h"

float cross(Vector2<float> p1, Vector2<float> p2){
  return p1.x()*p2.y()-p2.x()*p1.y();
}