#pragma once
#ifndef CELL_H_
#define CELL_H_

class Cell
{
public:
  enum CellState{ CS_CLOSED, CS_OPENING, CS_OPENED, CS_CLOSING };
  enum CellValue{ EMPTY, SELECTED };
private:
  static float animSpeed;
  float animPart; // анимация от 0.0 до 1.0
  enum CellState state;
  enum CellValue value;
public:
  Cell(void) : animPart(0.0), state(CS_CLOSED), value(EMPTY){}
  Cell(int val);
  ~Cell(void){}

  void update(float dt);
  void draw( int x, int y, int size );

  void setVal(int val);
  void setState(CellState s) { state = s; }
};

#endif