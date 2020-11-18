#pragma once
#include "Typedefs.h"
#include "IO.h"
#include "Time.h"
#include "Syscalls.h"
#include "Devices/VGA.h"
#include "Devices/Keyboard.h"
#include "Devices/PIC.h"

struct IDT64
{
	uint16 offset_low;
	uint16 selector;
	uint8 ist;
	uint8 types_attr;
	uint16 offset_mid;
	uint32 offset_high;
	uint32 zero;
};

extern void (*MainKeyboardHandler)(uint8 scanCode, uint8 chr);
void SetIDT(uint64 isr, uint8 num);
void InitializeIDT();