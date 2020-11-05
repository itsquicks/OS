#pragma once
#include "Typedefs.h"
#include "../IO.h"
#include "../Heap.h"

extern uint32* cpu_name;
extern uint64 cpu_speed;

void CPUID(uint32 code, uint32& eax, uint32& ebx, uint32& ecx, uint32& edx);
void Get_CPU_Name();
void Get_CPU_Speed();
uint64 RDTSC();