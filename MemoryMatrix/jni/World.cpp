#include "World.h"
#include "ResourceManager.h"
#include "AudioHelper.h"
#include "GameState.h"
#include "Cell.h"
#include "Settings.h"
#include <algorithm>

namespace{
  //Texture charTex;
  int borderSize = 10;
  float dx;
  int boardSize;
  int cellSize;
  int rows, cols;
  float boardX, boardY;
  enum State{ APPEAR, OPEN, SHOW, CLOSE, HIDDEN, OPEN_RESULT, SHOW_RESULT, LEAVE } state;
  Cell cells[ Settings::MAX_ROWS * Settings::MAX_COLS ];
}

void World::reshape(){
  //cellSize = std::min((GLHelper::getWidth()-borderSize)/cols, (GLHelper::getHeight()-borderSize)/rows) - borderSize;
  //boardSize = (cellSize + borderSize) * cols;
  //x = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
  //y = GLHelper::yToGl(( GLHelper::getHeight() - boardSize + borderSize ) / 2.0);
}

bool World::init(){
  rows = 2;
  cols = 2;
  cellSize = std::min((GLHelper::getWidth()-borderSize)/cols, (GLHelper::getHeight()-borderSize)/rows) - borderSize;
  boardSize = (cellSize + borderSize) * cols;
  boardX = GLHelper::xToGl(-boardSize); // board hidden to left
  boardY = GLHelper::yToGl(( GLHelper::getHeight() - boardSize + borderSize ) / 2.0);
  state = State::APPEAR;
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++)
      cells[i*cols+j].setVal( rand() % 2 );
  //ResourceManager::loadImage("res/character.png",&charTex);
  initLevel();
  return true;
}

void World::initLevel(){
  //score = 0;
}

void World::draw(bool isActive){
  GLHelper::setColor(0.6f,0.6f,1.0f);
  int devX = GLHelper::glToX(boardX);
  int devY = GLHelper::glToY(boardY);
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++){
      cells[i*cols+j].draw(
        devX + j*(cellSize+borderSize),
        devY + i*(cellSize+borderSize),
        cellSize);
    }
  if(!isActive)
    return;
  static char scoreText[16];
  GLHelper::setColor(0.f,1.f,0.f);
  sprintf(scoreText,"Score: %d", score);
  GLHelper::drawText(0,16,scoreText);
}

void World::update(float dt){
  switch(state){
    case APPEAR:{
      float targetX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
      boardX += 2.0*dt;
      if ( boardX > targetX ){
        boardX = targetX;
        state = OPEN;
      }
    }break;
    case OPEN:
      break;
    case SHOW:
      break;
    case CLOSE:
      break;
    case HIDDEN:
      break;
    case OPEN_RESULT:
      break;
    case SHOW_RESULT:
      break;
    case LEAVE:
      break;
  }
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
  int boardDevX = GLHelper::glToX(boardX);
  int boardDevY = GLHelper::glToY(boardY);
  int c = (x - boardDevX) / (cellSize + borderSize);
  int r = (y - boardDevY) / (cellSize + borderSize);
  if ( c >= 0 && c < cols && r >= 0 && r < rows )
    cells[ r * cols + c ].setState( Cell::CellState::CS_OPENED );
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
