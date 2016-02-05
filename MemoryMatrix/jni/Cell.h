#pragma once
#ifndef CELL_H_
#define CELL_H_

class Cell
{
public:
  enum CellState{ CS_CLOSED, CS_OPENING, CS_OPENED, CS_CLOSING };
  enum CellValue{ CV_EMPTY, CV_SELECTED };
private:
  static float animSpeed;
  float animPart; // �������� �� 0.0 �� 1.0
  bool  byUser; // ������ ���� ������� �������?
  enum CellState state;
  enum CellValue value;
public:
  Cell(void) : animPart(0.0), byUser(false), state(CS_CLOSED), value(CV_EMPTY){}
  Cell(int val);
  ~Cell(void){}

  void update(float dt);
  void draw( int x, int y, int size );

  CellValue getVal() const { return value; }
  void setVal(int val);
  CellState getState() const { return state; }
  void setState(CellState s, bool _byUser = false);
};

#endif