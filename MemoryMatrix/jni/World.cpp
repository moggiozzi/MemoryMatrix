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
  Cell cells[MAX_ROWS * MAX_COLS];
  int currentLevel;

  //������� ��� ������ ��� ������ - ������� �����
  //���� ������ - �������� �� ������� ������
  //������ ��� � �������� - ������� ����
  bool withMistake;
  bool prevWithMistake;

  int mistakeCount;
  const int MAX_MISTAKES = 3;

  uint score;
  const int MAX_TOP_SCORES = 5;
  uint topScores[MAX_TOP_SCORES]={0,0,0,0,0};

  const char * saveFile = "save.dat";
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
      boardX = GLHelper::xToGl(( GLHelper::getWidth() - boardSize ) / 2);
    break;
    case WS_SHOW_RESULT:
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
  load();
  withMistake = prevWithMistake = false;
  mistakeCount = 0;
  currentLevel = 1;
  score = 0;
  state = WS_APPEAR;
  initLevel();
  calcSizes();
  //ResourceManager::loadImage("res/character.png",&charTex);
  return true;
}

void World::initLevel(){
  prevWithMistake = withMistake;
  withMistake = false;
  rows = std::min(MAX_ROWS, ( currentLevel - 1 ) / 2 + 2);
  cols = std::min(MAX_COLS, currentLevel / 2 + 2);
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
  GLHelper::setColor(Settings::colorText);
  sprintf(text,"Level %d", currentLevel);
  GLHelper::drawText( 0, 16, text );

  sprintf(text,"Score %d", score);
  GLHelper::drawText( 0, 32, text );

  GLHelper::setColor(0.3f,0.3f,0.3f);
  for(int i=0;i<MAX_MISTAKES;i++){
    GLHelper::drawRect2d( 16 + i * 16, 48, 14, 14 );
  }
  GLHelper::setColor(1.f,0.f,0.f);
  for(int i=0;i<mistakeCount;i++){
    GLHelper::drawRect2d( 16 + i * 16, 48, 14, 14 );
  }
  if ( state == WS_SHOW_RESULT )
  {
    int textSize = GLHelper::getHeight() / (MAX_TOP_SCORES + 1);
    GLHelper::setColor(Settings::colorText);
    GLHelper::drawText( (GLHelper::getWidth() - 3 * textSize)/2, 0, "Top", textSize );
    for(int i=0;i<MAX_TOP_SCORES;i++){
      if(topScores[i]==score)
        GLHelper::setColor(Settings::colorResultScore);
      else
        GLHelper::setColor(Settings::colorText);
      sprintf(text,"%d.%4d", i+1, topScores[i]);
      GLHelper::drawText( (GLHelper::getWidth() - 6*textSize)/2, (i+1)*textSize, text, textSize );
    }
  }
}

void openAllCells(){
  for(int i=0; i<cellCount;i++){
    if ( cells[i].getState()==Cell::CS_CLOSED )
      cells[i].setState( Cell::CS_OPENING );
  }
}

void updateTopScore(){
  int minIdx = 0;
  for(int i=1;i<MAX_TOP_SCORES;i++)
    if ( topScores[i] < topScores[minIdx] )
      minIdx = i;
  topScores[minIdx] = score;
  // sort
  for(int i=0;i<MAX_TOP_SCORES;i++)
    for(int j=i+1;j<MAX_TOP_SCORES;j++){
      if(topScores[i]<topScores[j]){
        int tmp = topScores[i];
        topScores[i]=topScores[j];
        topScores[j]=tmp;
      }
    }
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
      if ( pt.CntSeconds() < 0.5 ) break;
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
      int inAnimation = false;
      for(int i=0; i<cellCount;i++){
        cells[i].update(dt);
        if ( cells[i].getVal()==Cell::CV_SELECTED && cells[i].getState()==Cell::CS_OPENED )
          openedCellCount++;
        if ( cells[i].getState() == Cell::CS_OPENING )
          inAnimation = true;
      }
      if ( openedCellCount >= cellCount / 2 || (!inAnimation && mistakeCount >= MAX_MISTAKES) ){
        openAllCells();
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
    case WS_LEAVE:
      pt.Stop();
      if ( pt.CntSeconds() > 1 )
      {
        boardX += 2*dt;
        if ( GLHelper::glToX( boardX ) > GLHelper::getWidth() )
        {
          if ( mistakeCount >= MAX_MISTAKES ){
            updateTopScore();
            save();
            state = WS_SHOW_RESULT;
            break;
          }
          if ( !withMistake )
            currentLevel++;
          else if ( prevWithMistake && currentLevel > 1 ) currentLevel--;
          initLevel();
          state = WS_APPEAR;
        }
      }
    break;
    case WS_SHOW_RESULT:
      break;
  }
}

void World::touch(int x, int y){
  switch(state){
    case WS_HIDDEN:{
      int boardDevX = GLHelper::glToX(boardX);
      int boardDevY = GLHelper::glToY(boardY);
      int c = (x - boardDevX - borderSize) / cellSize;
      int r = (y - boardDevY - borderSize) / cellSize;
      if ( x > boardDevX && y > boardDevY && c >= 0 && c < cols && r >= 0 && r < rows )
      {
        if ( cells[ r * cols + c ].getState() == Cell::CS_CLOSED ){
          cells[ r * cols + c ].setState( Cell::CS_OPENING, true );
          if ( cells[ r * cols + c ].getVal() == 0 )
          {
            withMistake = true;
			if (mistakeCount < MAX_MISTAKES)
              mistakeCount++;
          } else
            score += 1;
        }
      }
    }break;
    case WS_SHOW_RESULT:
      init();
      break;
  }
}

void World::save(){
  FILE *file = fopen(saveFile,"w");
  if (file==0)
    return;
  size_t n = fwrite( topScores, 1, sizeof(topScores), file );
  fclose(file);
}

void World::load(){
  FILE *file = fopen(saveFile,"r");
  if (file==0)
    return;
  size_t n = fread( topScores, 1, sizeof(topScores), file );
  fclose(file);
}
