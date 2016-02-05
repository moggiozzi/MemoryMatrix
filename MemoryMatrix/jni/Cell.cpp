#include "Cell.h"
#include "Settings.h"
#include "GLHelper.h"

float Cell::animSpeed = 2.0;

Cell::Cell(int val) : animPart(0.0), state(CS_CLOSED){
  (val==0) ? value = CV_EMPTY : value = CV_SELECTED;
}
void Cell::setVal(int val){
  (val==0) ? value = CV_EMPTY : value = CV_SELECTED;
}
void Cell::setState(CellState s) {
  if ( state != s ){
    animPart = 0;
    state = s;
  }
}
void Cell::update(float dt){
  switch(state){
    case CS_CLOSED:
    case CS_OPENED:
      break;
    case CS_OPENING:
      animPart += animSpeed*dt;
      if ( animPart >= 1.0 )
        state = CS_OPENED;
      break;
    case CS_CLOSING:
      animPart += animSpeed*dt;
      if ( animPart >= 1.0 ){
        state = CS_CLOSED;
        animPart = 1.0;
      }
      break;
  }
}

void Cell::draw( int x, int y, int size ){
  int w = size;
  switch(state){
    case CS_CLOSING:{
      if (animPart < 0.5)
        GLHelper::setColor(value == CV_EMPTY ? settings.colorCellEmpty : settings.colorCellFull);
      else
        GLHelper::setColor(settings.colorCellClosed);
      float k = (animPart <= 0.5) ? 1 - 2*animPart : 2*animPart-1;
      GLHelper::drawRect2d( x + (size - size*k)/2, y, size*k, size);
    }break;
    case CS_CLOSED:
      GLHelper::setColor(settings.colorCellClosed);
      GLHelper::drawRect2d( x, y, size, size);
      break;
    case CS_OPENING:{
      if (animPart < 0.5)
        GLHelper::setColor(settings.colorCellClosed);
      else
        GLHelper::setColor(value == CV_EMPTY ? settings.colorCellEmpty : settings.colorCellFull);
      float k = (animPart <= 0.5) ? 1 - 2*animPart : 2*animPart-1;
      GLHelper::drawRect2d( x + (size - size*k)/2, y, size*k, size);
      }break;
    case CS_OPENED:
      GLHelper::setColor(value == CV_EMPTY ? settings.colorCellEmpty : settings.colorCellFull);
      GLHelper::drawRect2d( x , y, size, size);
      break;
    //case CS_OPENING:
    //  break;
    //case CS_CLOSING:
    //  break;
  }
}