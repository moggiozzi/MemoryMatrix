#include "Fps.h"

Fps::Fps():currentSpanIdx(0),currentSpanCnt(0){
  for(uint i=0;i<maxSpansCnt;i++){
    spans[i]=0;
  }
}

void Fps::add(float span){
  spans[currentSpanIdx]=span;
  currentSpanIdx=(currentSpanIdx+1)%maxSpansCnt;
  if(currentSpanCnt<maxSpansCnt)
    currentSpanCnt++;
}

uint Fps::getFps(){
  float sum = 0;
  for(uint i=0;i<currentSpanCnt;i++)
    sum+=spans[i];
  if(sum>0)
    return static_cast<uint>(currentSpanCnt/sum);
  else
    return 0;
}
