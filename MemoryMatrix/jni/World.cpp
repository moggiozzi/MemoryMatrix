#include "World.h"
#include "ResourceManager.h"
#include "AudioHelper.h"
#include "GameState.h"
#include "Cell.h"
#include "Settings.h"
#include "PerformanceTimer.h"
#include <algorithm>

namespace{
  //Texture charTex;
  int borderSize = 10;
  int boardSize;
  int cellSize;
  int rows, cols, cellCount;
  float boardX, boardY;
  enum WorldState{ WS_APPEAR, WS_OPEN, WS_SHOW, WS_CLOSE, WS_HIDDEN, WS_OPEN_RESULT, WS_SHOW_RESULT, WS_LEAVE } state;
  Cell cells[ Settings::MAX_ROWS * Settings::MAX_COLS ];
}

void World::reshape(){
  cellSize = std::min((GLHelper::getWidth()-2*borderSize)/cols, (GLHelper::getHeight()-2*borderSize)/rows);
  boardSize = cellSize * cols + 2*borderSize;
  switch(state){
    case WS_OPEN:
    case WS_SHOW:
    case WS_CLOSE:
    case WS_HIDDEN:
    case WS_OPEN_RESULT:
    case WS_SHOW_RESULT:
    case WS_LEAVE:
      boardX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
      break;
  }
  boardY = GLHelper::yToGl(0);
}

bool World::init(){
  rows = 2;
  cols = 2;
  cellCount = rows * cols;
  cellSize = std::min((GLHelper::getWidth()-2*borderSize)/cols, (GLHelper::getHeight()-2*borderSize)/rows);
  boardSize = cellSize * cols;
  boardX = GLHelper::xToGl(-boardSize); // board hidden to left
  boardY = GLHelper::yToGl(0);
  state = WorldState::WS_APPEAR;
  //ResourceManager::loadImage("res/character.png",&charTex);
  initLevel();
  return true;
}

void World::initLevel(){
  int cnt = cellCount / 2;
  for(int i=0; i<cellCount;i++)
      cells[i].setVal( 0 );
  while(cnt>0){
    int i = rand()%cellCount;
    if(cells[i].getVal()==0){
      cells[i].setVal(1);
      cnt--;
    }
  }
}

void World::draw(bool isActive){
  GLHelper::setColor(0.6f,0.6f,1.0f);
  int devX = GLHelper::glToX(boardX);
  int devY = GLHelper::glToY(boardY);
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++){
      cells[i*cols+j].draw(
        devX + borderSize + j*cellSize + borderSize,
        devY + borderSize + i*cellSize + borderSize,
        cellSize - 2*borderSize);
    }
  if(!isActive)
    return;
  static char scoreText[16];
  GLHelper::setColor(0.f,1.f,0.f);
  sprintf(scoreText,"Score: %d", score);
  GLHelper::drawText(0,16,scoreText);
}

void World::update(float dt){
  static PerformanceTimer pt;
  switch(state){
    case WS_APPEAR:{
      float targetX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
      boardX += 2*dt;
      if ( boardX > targetX ){
        boardX = targetX;
        for(int i=0; i<cellCount;i++)
          cells[i].setState(Cell::CS_OPENING);
        state = WS_OPEN;
      }
    }break;
    case WS_OPEN:
      for(int i=0; i<cellCount;i++)
        cells[i].update(dt);
      if (cells[0].getState() == Cell::CS_OPENED ){
        state = WS_SHOW;
        pt.Start();
      }
      break;
    case WS_SHOW:
      pt.Stop();
      if(pt.CntSeconds()>2){
        for(int i=0; i<cellCount;i++)
          cells[i].setState(Cell::CS_CLOSING);
        state = WS_CLOSE;
      }
      break;
    case WS_CLOSE:
      for(int i=0; i<cellCount;i++)
        cells[i].update(dt);
      if (cells[0].getState() == Cell::CS_CLOSED ){
        state = WS_HIDDEN;
        pt.Start();
      }
      break;
    case WS_HIDDEN:
      break;
    case WS_OPEN_RESULT:
      break;
    case WS_SHOW_RESULT:
      break;
    case WS_LEAVE:
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
  int c = (x - boardDevX - borderSize) / cellSize;
  int r = (y - boardDevY - borderSize) / cellSize;
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
