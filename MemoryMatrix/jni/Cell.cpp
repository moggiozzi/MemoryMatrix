#include "Cell.h"
#include "Settings.h"
#include "GLHelper.h"

float Cell::animSpeed = 2.0;

Cell::Cell(int val) : animPart(0.0), state(CS_CLOSED){
  (val==0) ? value = EMPTY : value = SELECTED;
}
void Cell::setVal(int val){
  (val==0) ? value = EMPTY : value = SELECTED;
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
      if ( animPart >= 1.0 )
        state = CS_CLOSED;
      break;
  }
}

void Cell::draw( int x, int y, int size ){
  int w = size;
  switch(state){
    case CS_CLOSING: //todo
    case CS_CLOSED:
      GLHelper::setColor(settings.colorCellClosed);
      GLHelper::drawRect2d( x, y, x+size, y+size);
      break;
    case CS_OPENING: //todo
    case CS_OPENED:
      if (value == EMPTY)
        GLHelper::setColor(settings.colorCellEmpty);
      else
        GLHelper::setColor(settings.colorCellFull);
      GLHelper::drawRect2d( x, y, x+size, y+size);
      break;
    //case CS_OPENING:
    //  break;
    //case CS_CLOSING:
    //  break;
  }
}