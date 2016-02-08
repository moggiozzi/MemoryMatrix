#ifndef WORLD_H_
#define WORLD_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif

class World{
public:
  World() {}
  ~World(){}

  bool init();
  void initLevel();
  void draw(bool isActive=true);
  void update(float dt);
  // Изменение размера окна
  void reshape();
  //bool keyDown(uint keyCode);
  void touch(int x, int y);
  //void accel(float x, float y, float z);
  
  void save();
  void load();
};

#endif // WORLD_H_
