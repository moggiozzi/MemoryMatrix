#include "PerformanceTimer.h"
#include <limits.h>

#ifdef _WIN32
bool PerformanceTimer::isInitizalied = false;
LARGE_INTEGER PerformanceTimer::ticksPerSec;
#endif

/**
* Возвращает разницу между двумя uint64_t числами с учетом переполнения
* \param [in] first  первая метка времени
* \param [in] second вторая метка времени
* \return возвращает результат операции second - first
*/
static inline 
uint64_t Uint64OverflowDiff( const uint64_t& first, const uint64_t& second )
{
  uint64_t res = 0;
  if (second >= first)
    res = second - first;
  else
    res = ULLONG_MAX - second + first;
  return res;
}

#ifdef __linux__
/// Вычисляет разницу между двумя timespec
timespec timespec_diff( const timespec& start, const timespec& end )
{
  timespec temp;
  if ( end.tv_nsec < start.tv_nsec ) 
  {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  } 
  else 
  {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}
#endif

PerformanceTimer::PerformanceTimer()
{
  #if defined(_WIN32)
  if ( !isInitizalied )
  {
    isInitizalied = true;
    BOOL res = QueryPerformanceFrequency(&PerformanceTimer::ticksPerSec);
    #ifdef DEBUG
    if (res == FALSE)
      throw Exception(LINE_INFO);
    #endif
  }
  ticks1_.QuadPart = 0;
  ticks2_.QuadPart = 0;

  #elif defined(__linux__)
  beg_.tv_sec = 0;
  beg_.tv_nsec = 0;
  end_.tv_sec = 0;
  end_.tv_nsec = 0;
  #endif
  isStarted_ = false;
}
PerformanceTimer::~PerformanceTimer()
{
}

void PerformanceTimer::Start()
{
  #if defined(_WIN32)
  BOOL res = QueryPerformanceCounter(&ticks1_);
  #ifdef DEBUG
  if (res == FALSE)
    throw Exception(LINE_INFO);
  #endif  

  #elif defined(__linux__)
  int res = clock_gettime( CLOCK_MONOTONIC, &beg_ );
  #ifdef DEBUG
  if ( res == -1 )
    throw Exception(LINE_INFO);
  #endif  
  #endif
  isStarted_ = true;
}

void PerformanceTimer::Stop()
{
  #if defined(_WIN32)
  BOOL res = QueryPerformanceCounter(&ticks2_);
  #ifdef DEBUG
  if (res == FALSE)
    throw Exception(LINE_INFO);
  #endif

  #elif defined(__linux__)
  int res = clock_gettime( CLOCK_MONOTONIC, &end_ );
  #ifdef DEBUG
  if ( res == -1 )
    throw Exception(LINE_INFO);
  #endif  
  #endif
  isStarted_ = false;
}

void PerformanceTimer::Restart()
{
  this->Start();
}

double PerformanceTimer::CntSeconds() const
{
  #if defined(_WIN32)
  LARGE_INTEGER diff;
  diff.QuadPart = Uint64OverflowDiff( ticks1_.QuadPart, ticks2_.QuadPart );  
  double seconds = (double) diff.QuadPart / ticksPerSec.QuadPart;
  return seconds;
  
  #elif defined(__linux__)
  timespec diff = timespec_diff( beg_, end_ );
  double seconds = diff.tv_sec + diff.tv_nsec / 1000000000;
  return seconds;
  
  #else
  return 0.0;
  #endif
}

double PerformanceTimer::CntMilliseconds() const
{
  #if defined(_WIN32)
  LARGE_INTEGER diff;
  diff.QuadPart = Uint64OverflowDiff( ticks1_.QuadPart, ticks2_.QuadPart );  
  double milliSeconds = (double) diff.QuadPart * 1000 / ticksPerSec.QuadPart;
  return milliSeconds;
  
  #elif defined(__linux__)
  timespec diff = timespec_diff( beg_, end_ );
  double milliSeconds = diff.tv_sec * 1000 + diff.tv_nsec / 1000000;  
  return milliSeconds;
  #else
  return 0.0;
  #endif
}

double PerformanceTimer::CntMicroseconds() const
{
  #if defined(_WIN32)
  LARGE_INTEGER diff;
  diff.QuadPart = Uint64OverflowDiff( ticks1_.QuadPart, ticks2_.QuadPart );  
  double microSeconds = (double) diff.QuadPart * 1000000 / ticksPerSec.QuadPart;
  return microSeconds;
  
  #elif defined(__linux__)
  timespec diff = timespec_diff( beg_, end_ );
  double microSeconds = (int64)diff.tv_sec * 1000000 + diff.tv_nsec / 1000;
  return microSeconds;
  #else
  return 0.0;
  #endif
}

uint32_t PerformanceTimer::CntMicrosecondsUint() const
{
  #if defined(_WIN32)
  LARGE_INTEGER diff;
  diff.QuadPart = Uint64OverflowDiff( ticks1_.QuadPart, ticks2_.QuadPart );  
  uint32_t microSeconds =  (uint32_t) ( (double)diff.QuadPart * 1000000 / ticksPerSec.QuadPart );
  return microSeconds;
  
  #elif defined(__linux__)
  timespec diff = timespec_diff( beg_, end_ );
  uint microSeconds = (uint) ( (int64)diff.tv_sec * 1000000 + diff.tv_nsec / 1000 );
  return microSeconds;
  #else
  return 0;
  #endif
}

uint64_t PerformanceTimer::CntTicks() const
{
  #if defined(_WIN32)
  LARGE_INTEGER diff;
  diff.QuadPart = Uint64OverflowDiff( ticks1_.QuadPart, ticks2_.QuadPart );  
  return diff.QuadPart;
  
  #elif defined(__linux__)
  return this->CntMicrosecondsUint(); //тиков нету
  #else
  return 0;
  #endif
}

bool PerformanceTimer::IsElapsed_Ms( uint32_t milliseconds ) const
{
  #if defined(_WIN32)
  LARGE_INTEGER ticks3;
  BOOL res = QueryPerformanceCounter( &ticks3 );
  #ifdef DEBUG
  if (res == FALSE)
    throw Exception(LINE_INFO);
  #endif
  LARGE_INTEGER diff;
  diff.QuadPart = Uint64OverflowDiff( ticks1_.QuadPart, ticks3.QuadPart );  
  uint32_t elapsedMs =  (uint32_t) ( (double)diff.QuadPart * 1000 / ticksPerSec.QuadPart );
  if ( elapsedMs >= milliseconds )  
    return true;
  return false;

  #elif defined(__linux__)
  timespec timeSpec;
  int res = clock_gettime( CLOCK_MONOTONIC, &timeSpec );
  #ifdef DEBUG
  if ( res == -1 )
    throw Exception(LINE_INFO);
  #endif  
  timespec diff = timespec_diff( beg_, timeSpec );
  uint elapsedMs = diff.tv_sec * 1000 + diff.tv_nsec / 1000000;  
  if ( elapsedMs >= milliseconds )  
    return true;
  return false;

  #else
  return false;
  #endif
}
