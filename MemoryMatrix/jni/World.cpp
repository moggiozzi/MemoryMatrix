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
  int boardSize;
  int cellSize;
  int rows, cols;
  float boardX, boardY;
  enum State{ APPEAR, OPEN, SHOW, CLOSE, HIDDEN, OPEN_RESULT, SHOW_RESULT, LEAVE } state;
  Cell cells[ Settings::MAX_ROWS * Settings::MAX_COLS ];
}

void World::reshape(){
  cellSize = std::min((GLHelper::getWidth()-2*borderSize)/cols, (GLHelper::getHeight()-2*borderSize)/rows);
  boardSize = cellSize * cols + 2*borderSize;
  boardX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
  boardY = GLHelper::yToGl(0);
}

bool World::init(){
  rows = 2;
  cols = 2;
  cellSize = std::min((GLHelper::getWidth()-2*borderSize)/cols, (GLHelper::getHeight()-2*borderSize)/rows);
  boardSize = cellSize * cols;
  boardX = GLHelper::xToGl(-boardSize); // board hidden to left
  boardY = GLHelper::yToGl(0);
  state = State::APPEAR;
  //ResourceManager::loadImage("res/character.png",&charTex);
  initLevel();
  return true;
}

void World::initLevel(){
  int cnt = rows * cols / 2;
  for(int i=0;i<rows;i++)
    for(int j=0;j<cols;j++)
      cells[i*cols+j].setVal( 0 );
  while(cnt>0){
    int i = rand()%rows;
    int j = rand()%cols;
    if(cells[i*cols+j].getVal()==0){
      cells[i*cols+j].setVal(1);
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
  switch(state){
    case APPEAR:{
      float targetX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
      boardX += 2*dt;
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
