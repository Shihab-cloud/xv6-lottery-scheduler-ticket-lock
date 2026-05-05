#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat {
  int inuse[NPROC];   // Whether this slot of the process table is in use (1 or 0)
  int tickets[NPROC]; // The number of tickets this process has
  int pid[NPROC];     // The PID of each process
  int ticks[NPROC];   // The number of ticks each process has accumulated
};

#endif