#ifndef FPS_H_
#define FPS_H_

#ifndef GLOBAL_H_
#include "Global.h"
#endif

class Fps{
#define maxSpansCnt 100
  float spans[maxSpansCnt];
  uint currentSpanIdx;
  uint currentSpanCnt;
public:
  Fps();
  void add(float span);
  uint getFps();
};

#endif