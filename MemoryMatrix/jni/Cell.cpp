#include "Cell.h"

float Cell::animSpeed = 2.0;

void Cell::update(float dt){
  switch(state){
    case CLOSED:
    case OPENED:
      break;
    case OPENING:
      animPart += animSpeed*dt;
      if ( animPart >= 1.0 )
        state = OPENED;
      break;
    case CLOSING:
      animPart += animSpeed*dt;
      if ( animPart >= 1.0 )
        state = CLOSED;
      break;
  }
}

void Cell::draw( int x, int y, int size ){
  int w = size;
  switch(state){
    case CLOSED:
      break;
    case OPENED:
      break;
    case OPENING:
      break;
    case CLOSING:
      break;
  }
}