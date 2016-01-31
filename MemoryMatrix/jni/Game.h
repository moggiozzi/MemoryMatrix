#ifndef GAME_H_
#define GAME_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif
#ifndef WORLD_H_
#include "World.h"
#endif
#ifndef MENU_H_
#include "Menu.h"
#endif
#ifndef GAMESTATE_H_
#include "GameState.h"
#endif
#ifndef FPS_H_
#include "Fps.h"
#endif

class Game{
  bool isInit;
  World world;
  Menu menu;
  Fps fps;
  void drawFps();
public:
  Game();
  bool init();
  void draw();
  void update(float dt);
  void save();
  void resume();
  void reshape();
  void touch(int x, int y);
  bool keyDown(uint keyCode);
  void accel(float x, float y, float z);
  void save(const char* fileName);
  void load(const char* fileName);
  uint getSaveDataSize();
  void saveTo(char *data);
  void loadFrom(const char *data, const char * const dataEnd);
};

#endif // GAME_H_
