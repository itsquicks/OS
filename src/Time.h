#pragma once
#include "Typedefs.h"
#include "IO.h"
#include "Devices/VGA.h"
//#include "Devices/CMOS.h"
//#include "Devices/PIT.h"
#include "Devices/PIC.h"

extern uint64 irq0_count;

void PrintTime();
void PrintDate();
void Sleep(uint64 ms);