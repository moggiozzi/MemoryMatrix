#include "Game.h"

#include "GLHelper.h"
#include "ResourceManager.h"
#include "AudioHelper.h"
#include "GameState.h"

Game::Game(){}

bool Game::init(){
  isInit = true;
  setGameState(GS_MENU);
  isInit = isInit && world.init();
  int sId;
  //isInit = isInit && AudioHelper::open("res/main.ogg",sId);
  //AudioHelper::update();
  //AudioHelper::play(sId,false); // todo true
  //ResourceManager::loadImage("res/bg.png",&bgTex);
  return isInit;
}

void Game::drawFps(){
  static char str[16];
  sprintf(str,"fps:%d",fps.getFps());
  GLHelper::setColor(1.f,1.f,1.f);
  GLHelper::drawText(0, 16, str);
}

void Game::draw(){
  GLHelper::clear(0, 0, 0);
  GameState gState = getGameState();
  switch (gState)
  {
  case GS_INGAME:
    world.draw();
    break;
  case GS_PAUSE:
    menu.draw();
    break;
  case GS_MENU: //case GS_GAMEOVER:
    menu.draw();
    break;
  }
  //drawFps();
  GLHelper::swapBuffers();
}

void Game::update(float dt){
  if ( dt <= 0 ) // ???
    return;
  if(dt>0.06f)
    dt=0.06f; // Не хватает ресурсов: меньше 16кадр/сек
  fps.add(dt);
  GameState gState = getGameState();
  switch (gState){
  case GS_INGAME:
    if (getPrevGameState()!=gState){
      world.init();
      setGameState(gState); // reset prevGameState
    }
    world.update(dt);
    break;
  }
}
void Game::reshape(){ world.reshape(); }
bool Game::keyDown(uint keyCode){
  GameState gState = getGameState();
  if ( keyCode == KEY_ESC){
    if (gState == GS_MENU)
      exit(0);
    setGameState(GS_MENU);
  }
  return true;
}

void Game::touch(int x, int y){
  GameState gState = getGameState();
  switch (gState){
  case GS_INGAME:
      world.touch(x,y);
    break;
  default:
      menu.touch(x,y);
    break;
  }
}

void Game::accel(float x, float y, float z){
}

uint Game::getSaveDataSize(){
  return 0;
}
void Game::saveTo(char* data){
}
void Game::loadFrom(const char *data, const uint len){
}

void Game::save(const char* fileName){
  if ( getGameState() == GS_INGAME )
    setGameState( GS_PAUSE );
  FILE *file = fopen(fileName,"w");
  if (file==0) return;
  uint size = getSaveDataSize();
  char *buf = new char[size];
  saveTo(buf);
  fwrite(buf,1,size,file);
  fclose(file);
  delete[] buf;
}
void Game::load(const char* fileName){
  FILE *file = fopen(fileName,"r");
  if (file==0) return;
  fseek (file , 0 , SEEK_END);
  uint size = ftell(file);
  rewind (file);
  char *buf = new char[size];
  fread(buf,1,size,file);
  loadFrom( buf, size );
  fclose(file);
  delete[] buf;
}
