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
  int currentLevel;

  //ќткрыты все €чейки без ошибок - уровень вверх
  //Ѕыли ошибки - остатьс€ на текущем уровне
  //¬торой раз с ошибками - уровень вниз
  bool withMistake;
  bool prevWithMistake;
}

void calcSizes(){
  cellCount = rows * cols;
  cellSize = std::min((GLHelper::getWidth()-2*borderSize)/cols, (GLHelper::getHeight()-2*borderSize)/rows);
  boardSize = cellSize * cols;
  boardY = GLHelper::yToGl(0);
  cellSize = std::min((GLHelper::getWidth()-2*borderSize)/cols, (GLHelper::getHeight()-2*borderSize)/rows);
  borderSize = cellSize / 20 + 1;
  boardSize = cellSize * cols + 2*borderSize;
  switch(state){
    case WS_OPEN:
    case WS_SHOW:
    case WS_CLOSE:
    case WS_HIDDEN:
    case WS_OPEN_RESULT:
    case WS_SHOW_RESULT:
      boardX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
    break;
    case WS_APPEAR:
    case WS_LEAVE:
      boardX = GLHelper::xToGl(-boardSize); // board hidden to left
      break;
  }
}

void World::reshape(){
  calcSizes();
}

bool World::init(){
  withMistake = prevWithMistake = false;
  currentLevel = 1;
  initLevel();
  calcSizes();
  state = WorldState::WS_APPEAR;
  //ResourceManager::loadImage("res/character.png",&charTex);
  return true;
}

void World::initLevel(){
  prevWithMistake = withMistake;
  withMistake = false;
  rows = ( currentLevel - 1 ) / 2 + 2;
  cols = currentLevel / 2 + 2;
  calcSizes();
  for(int i=0; i<cellCount;i++){
    cells[i].setVal( 0 );
    cells[i].setState( Cell::CS_CLOSED );
  }

  int cnt = cellCount / 2;
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
  static char text[16];
  GLHelper::setColor(0.f,1.f,0.f);
  sprintf(text,"Level %d", currentLevel);
  GLHelper::drawText( 0, 16, text );

  sprintf(text,"Score %d", score);
  GLHelper::drawText( 0, 32, text );
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
        pt.Start();
      }
    }break;
    case WS_OPEN:
      pt.Stop();
      if ( pt.CntSeconds() < 1 ) break;
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
    case WS_HIDDEN:{
      int openedCellCount = 0;
      for(int i=0; i<cellCount;i++){
        cells[i].update(dt);
        if ( cells[i].getState()==Cell::CS_OPENED )
          openedCellCount++;
      }
      if ( openedCellCount >= cellCount / 2 ){
        for(int i=0; i<cellCount;i++){
          if ( cells[i].getState()==Cell::CS_CLOSED )
            cells[i].setState( Cell::CS_OPENING );
        }
        state = WS_OPEN_RESULT;
      }
    }break;
    case WS_OPEN_RESULT: {
      bool isAllOpened = true;
      for(int i=0; i<cellCount;i++){
        cells[i].update(dt);
        if( cells[i].getState() != Cell::CS_OPENED )
          isAllOpened = false;
      }
      if ( isAllOpened ){
        pt.Start();
        state = WS_LEAVE;
      }
    } break;
    //case WS_SHOW_RESULT:
    //  break;
    case WS_LEAVE:
      pt.Stop();
      if ( pt.CntSeconds() > 1 )
      {
        boardX += 2*dt;
        if ( GLHelper::glToX( boardX ) > GLHelper::getWidth() )
        {
          if ( !withMistake )
            currentLevel++;
          else if ( prevWithMistake && currentLevel > 1 ) currentLevel--;
          initLevel();
          boardX = GLHelper::xToGl(-boardSize); // board hidden to left
          state = WS_APPEAR;
        }
      }
    break;
  }
}

void World::touch(int x, int y){
  LOGI("x:%d y:%d",x,y);
  switch(state){
    case WS_HIDDEN:{
      int boardDevX = GLHelper::glToX(boardX);
      int boardDevY = GLHelper::glToY(boardY);
      int c = (x - boardDevX - borderSize) / cellSize;
      int r = (y - boardDevY - borderSize) / cellSize;
      if ( x > boardDevX && y > boardDevY && c >= 0 && c < cols && r >= 0 && r < rows )
      {
        if ( cells[ r * cols + c ].getState() == Cell::CS_CLOSED ){
          cells[ r * cols + c ].setState( Cell::CellState::CS_OPENING, true );
          if ( cells[ r * cols + c ].getVal() == 0 )
          {
            score -= 10;
            withMistake = true;
          } else
            score += 10;
        }
      }
    }break;
  }
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
