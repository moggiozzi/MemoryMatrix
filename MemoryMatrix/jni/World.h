#ifndef WORLD_H_
#define WORLD_H_

#ifndef GLHELPER_H_
#include "GLHelper.h"
#endif

class World{
  uint score;
public:
  World() {}
  ~World(){}
  //Vector2f getSize(){return size;}
  bool init();
  void initLevel();
  void draw(bool isActive=true);
  void update(float dt);
  // Изменение размера окна
  void reshape();
  bool keyDown(uint keyCode);
  void touch(int x, int y);
  void accel(float x, float y, float z);
  
  uint getSaveDataSize();
  void saveTo(char *data);
  void loadFrom(const char *data, const char * const dataEnd);
};

#endif // WORLD_H_
