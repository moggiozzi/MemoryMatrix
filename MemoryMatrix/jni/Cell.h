#pragma once

class Cell
{
  static float animSpeed;
  float animPart; // анимация от 0.0 до 1.0
  enum State{ CLOSED, OPENING, OPENED, CLOSING } state;
  enum Value{ EMPTY, SELECTED } value;
public:
  Cell(void) : animPart(0.0), state(CLOSED), value(EMPTY){}
  ~Cell(void){}

  void update(float dt);
  void draw( int x, int y, int size );
};

