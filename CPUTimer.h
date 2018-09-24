#ifndef CPUTIMER_H
#define CPUTIMER_H

#include <time.h>
#include <iostream>
using namespace std;

class CPUTimer {
private:
  clock_t tick_count;
    
public:
  CPUTimer(void);
  void reset(void);
  double cur_CPUTime(void);
};


class AutoCPUTimer : public CPUTimer {
public:
  ~AutoCPUTimer(void);
};


CPUTimer::CPUTimer(void)
{
  tick_count = clock();
}

void CPUTimer::reset(void)
{ 
  tick_count = clock();
}

double CPUTimer::cur_CPUTime(void)
{
  return double(clock() - tick_count) / CLOCKS_PER_SEC;
}

AutoCPUTimer::~AutoCPUTimer(void)
{
  cerr << cur_CPUTime() << endl;
}

#endif

