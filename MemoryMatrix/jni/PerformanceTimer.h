#ifndef PERFORMANCE_TIMER_H
#define PERFORMANCE_TIMER_H

//#include "inc_lib/alias.h"
#include <stdint.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#elif defined(__linux__)
#include <ctime>
#endif


/**
* —четчик производительности дл€ скорости участков кода
*/ 
class PerformanceTimer
{
private:
  #if defined(_WIN32)
  static bool isInitizalied;
  static LARGE_INTEGER ticksPerSec;

  LARGE_INTEGER ticks1_;
  LARGE_INTEGER ticks2_;

  #elif defined(__linux__)
  timespec beg_;
  timespec end_;
  #endif
  bool isStarted_;

public:
  PerformanceTimer();
  ~PerformanceTimer();

  bool IsStarted() const
  {
    return isStarted_;
  }

  void Start();

  void Stop();

  void Restart();

  double CntSeconds() const;

  double CntMilliseconds() const;

  double CntMicroseconds() const;

  uint32_t CntMicrosecondsUint() const;

  uint64_t CntTicks() const;

  bool IsElapsed_Ms( uint32_t milliseconds ) const;
};

#endif
