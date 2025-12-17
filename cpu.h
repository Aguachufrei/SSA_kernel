#ifndef CPU_H
#define CPU_H
#include <stdio.h>
#include <stdlib.h>
#include "prozesuak.h"
extern struct CPU cpu;
struct CPU {
	int coreNum;
	int hthreadNum;
	struct PCB ***cores;
};
void cpu_inicialize(int coreNum, int hthreadNum);
#endif
