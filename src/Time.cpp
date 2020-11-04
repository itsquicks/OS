#include "Time.h"

uint64 irq0_count;

void PrintTime()
{
	//Print(HexToString(hour), lightcyanF);
	Print(':', lightcyanF);
	//Print(HexToString(minute), lightcyanF);
	Print(':', lightcyanF);
	//Print(HexToString(second), lightcyanF);
}

void PrintDate()
{
	//Print(HexToString(day), lightcyanF);
	Print('.', lightcyanF);
	//Print(HexToString(month), lightcyanF);
	Print(".", lightcyanF);
	//Print(IntToString(century), lightcyanF);
	//Print(HexToString(year), lightcyanF);
}

void Sleep(uint64 ms)
{
	irq0_count = 0;
	//InitializePIT(1000);

	uint8 mask = inb(PIC1_DATA);
	mask &= 0b11111110;
	SetMaskPIC1(mask);

	while (irq0_count < ms);

	mask |= 0b00000001;
	SetMaskPIC1(mask);

	return;
}