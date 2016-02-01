#include "Game.h"

#include "GLHelper.h"
#include "ResourceManager.h"
#include "AudioHelper.h"
#include "GameState.h"

Game::Game(){}
Texture bgTex;
bool Game::init(){
  isInit = true;
  setGameState(GS_MENU);
  isInit = isInit && menu.init();
  isInit = isInit && world.init();
  int sId;
  //isInit = isInit && AudioHelper::open("res/main.ogg",sId);
  AudioHelper::update();
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
    world.draw(true);
    menu.draw();
    break;
  case GS_MENU: case GS_GAMEOVER:
    world.draw(false);
    menu.draw();
    break;
  }
  //drawFps();
  GLHelper::swapBuffers();
}

void Game::update(float dt){
  world.update(dt);
  return;
  if ( dt <= 0 ) // ???
    return;
  if(dt>0.06f)
    dt=0.06f; // Не хватает ресурсов: меньше 16кадр/сек
  fps.add(dt);
  GameState gState = getGameState();
  switch (gState){
  case GS_INITLEVEL:
    world.initLevel();
    setGameState( GS_INGAME );
    break;
  case GS_INGAME:
    world.update(dt);
    break;
  case GS_MENU:
    if( gState != getPrevGameState() ){
      setGameState(gState); // to change prevGameState
      world.initLevel();
    }break;
  default:
    menu.update(dt);
    break;
  }
}
void Game::reshape(){ world.reshape(); }
bool Game::keyDown(uint keyCode){
  GameState gState = getGameState();
  switch (gState){
  case GS_INGAME:
    return world.keyDown(keyCode);
  break;
  default:
    return menu.keyDown(keyCode);
  break;
  }
}

void Game::touch(int x, int y){
  world.touch(x,y);
  //GameState gState = getGameState();
  //switch (gState){
  //case GS_INGAME:
  //    world.touch(x,y);
  //  break;
  //default:
  //    menu.touch(x,y);
  //  break;
  //}
}

void Game::accel(float x, float y, float z){
  world.accel(x,y,z);
}

uint Game::getSaveDataSize(){
  return world.getSaveDataSize()+sizeof(char);
}
void Game::saveTo(char* data){
  data[0] = getGameState();
  if ( data[0] == GS_PAUSE )
    world.saveTo(&data[1]);
}
void Game::loadFrom(const char *data, const char * const dataEnd){
  if ( data == dataEnd ) {
    return;
  }
  char state = data[0];
  if ( state == GS_PAUSE ){
    setGameState( GS_PAUSE );
    world.loadFrom(&data[1], dataEnd);
  }
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
  const char * const dataEnd = &buf[0]+size;
  loadFrom( buf, dataEnd );
  fclose(file);
  delete[] buf;
}
