#include "World.h"
#include "ResourceManager.h"
#include "AudioHelper.h"
#include "GameState.h"

namespace{
  //Texture charTex;
}

bool World::init(){
  //ResourceManager::loadImage("res/character.png",&charTex);
  initLevel();
  return true;
}

void World::initLevel(){
  //score = 0;
}

void World::draw(bool isActive){
  GLHelper::setColor(0.6f,0.6f,1.0f);
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++){
      GLHelper::drawRect2d( j*100, i*100, j*100+90, i*100+90);
    }
  if(!isActive)
    return;
  static char scoreText[16];
  GLHelper::setColor(0.f,1.f,0.f);
  sprintf(scoreText,"Score: %d", score);
  GLHelper::drawText(0,16,scoreText);
}

void World::update(float dt){
}

bool World::keyDown(uint keyCode){
  if (keyCode == KEY_ESC){
    setGameState(GS_PAUSE);
    return true;
  }
  switch(keyCode){
  case KEY_LEFT:
    return true;
    break;
  case KEY_RIGHT:
    return true;
    break;
  }
  return false;
}

void World::touch(int x, int y){
  if(x<GLHelper::getWidth()/2)
    keyDown(KEY_LEFT);
  else
    keyDown(KEY_RIGHT);
}

void World::accel(float x, float y, float z){
}

uint World::getSaveDataSize(){
  return 1;
}
void World::saveTo(char *data){
  data[0] = 0xFA;
}
void World::loadFrom(const char *data, const char * const dataEnd){
  int s = 1;
  if (dataEnd < data+1) {
    LOGI("Error: World::loadFrom() not valid data");
    return;
  }
}
