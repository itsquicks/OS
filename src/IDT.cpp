#include "IDT.h"

extern IDT64 _idt[256];

extern uint64 isr0;
extern uint64 isr1;
extern uint64 isr3;
extern uint64 isr4;
extern uint64 isr5;
extern uint64 isr6;
extern uint64 isr7;
extern uint64 isr8;
extern uint64 isr9;
extern uint64 isr10;
extern uint64 isr11;
extern uint64 isr12;
extern uint64 isr13;
extern uint64 isr14;
extern uint64 isr16;
extern uint64 isr17;
extern uint64 isr18;
extern uint64 isr19;

extern "C" void LoadIDT();

void (*MainKeyboardHandler)(uint8 scanCode, uint8 chr);

void InitializeIDT()
{
	_idt[0].zero = 0;
	_idt[0].offset_low = (uint16)(((uint64)&isr0 & 0x000000000000ffff));
	_idt[0].offset_mid = (uint16)(((uint64)&isr0 & 0x00000000ffff0000) >> 16);
	_idt[0].offset_high = (uint32)(((uint64)&isr0 & 0xffffffff00000000) >> 32);
	_idt[0].ist = 0;
	_idt[0].selector = 0x08;
	_idt[0].types_attr = 0x8e;

	_idt[1].zero = 0;
	_idt[1].offset_low = (uint16)(((uint64)&isr1 & 0x000000000000ffff));
	_idt[1].offset_mid = (uint16)(((uint64)&isr1 & 0x00000000ffff0000) >> 16);
	_idt[1].offset_high = (uint32)(((uint64)&isr1 & 0xffffffff00000000) >> 32);
	_idt[1].ist = 0;
	_idt[1].selector = 0x08;
	_idt[1].types_attr = 0x8e;

	_idt[3].zero = 0;
	_idt[3].offset_low = (uint16)(((uint64)&isr3 & 0x000000000000ffff));
	_idt[3].offset_mid = (uint16)(((uint64)&isr3 & 0x00000000ffff0000) >> 16);
	_idt[3].offset_high = (uint32)(((uint64)&isr3 & 0xffffffff00000000) >> 32);
	_idt[3].ist = 0;
	_idt[3].selector = 0x08;
	_idt[3].types_attr = 0x8e;

	_idt[4].zero = 0;
	_idt[4].offset_low = (uint16)(((uint64)&isr4 & 0x000000000000ffff));
	_idt[4].offset_mid = (uint16)(((uint64)&isr4 & 0x00000000ffff0000) >> 16);
	_idt[4].offset_high = (uint32)(((uint64)&isr4 & 0xffffffff00000000) >> 32);
	_idt[4].ist = 0;
	_idt[4].selector = 0x08;
	_idt[4].types_attr = 0x8e;

	_idt[5].zero = 0;
	_idt[5].offset_low = (uint16)(((uint64)&isr5 & 0x000000000000ffff));
	_idt[5].offset_mid = (uint16)(((uint64)&isr5 & 0x00000000ffff0000) >> 16);
	_idt[5].offset_high = (uint32)(((uint64)&isr5 & 0xffffffff00000000) >> 32);
	_idt[5].ist = 0;
	_idt[5].selector = 0x08;
	_idt[5].types_attr = 0x8e;

	_idt[6].zero = 0;
	_idt[6].offset_low = (uint16)(((uint64)&isr6 & 0x000000000000ffff));
	_idt[6].offset_mid = (uint16)(((uint64)&isr6 & 0x00000000ffff0000) >> 16);
	_idt[6].offset_high = (uint32)(((uint64)&isr6 & 0xffffffff00000000) >> 32);
	_idt[6].ist = 0;
	_idt[6].selector = 0x08;
	_idt[6].types_attr = 0x8e;

	_idt[7].zero = 0;
	_idt[7].offset_low = (uint16)(((uint64)&isr7 & 0x000000000000ffff));
	_idt[7].offset_mid = (uint16)(((uint64)&isr7 & 0x00000000ffff0000) >> 16);
	_idt[7].offset_high = (uint32)(((uint64)&isr7 & 0xffffffff00000000) >> 32);
	_idt[7].ist = 0;
	_idt[7].selector = 0x08;
	_idt[7].types_attr = 0x8e;

	_idt[8].zero = 0;
	_idt[8].offset_low = (uint16)(((uint64)&isr8 & 0x000000000000ffff));
	_idt[8].offset_mid = (uint16)(((uint64)&isr8 & 0x00000000ffff0000) >> 16);
	_idt[8].offset_high = (uint32)(((uint64)&isr8 & 0xffffffff00000000) >> 32);
	_idt[8].ist = 0;
	_idt[8].selector = 0x08;
	_idt[8].types_attr = 0x8e;

	_idt[9].zero = 0;
	_idt[9].offset_low = (uint16)(((uint64)&isr9 & 0x000000000000ffff));
	_idt[9].offset_mid = (uint16)(((uint64)&isr9 & 0x00000000ffff0000) >> 16);
	_idt[9].offset_high = (uint32)(((uint64)&isr9 & 0xffffffff00000000) >> 32);
	_idt[9].ist = 0;
	_idt[9].selector = 0x08;
	_idt[9].types_attr = 0x8e;

	_idt[10].zero = 0;
	_idt[10].offset_low = (uint16)(((uint64)&isr10 & 0x000000000000ffff));
	_idt[10].offset_mid = (uint16)(((uint64)&isr10 & 0x00000000ffff0000) >> 16);
	_idt[10].offset_high = (uint32)(((uint64)&isr10 & 0xffffffff00000000) >> 32);
	_idt[10].ist = 0;
	_idt[10].selector = 0x08;
	_idt[10].types_attr = 0x8e;

	_idt[11].zero = 0;
	_idt[11].offset_low = (uint16)(((uint64)&isr11 & 0x000000000000ffff));
	_idt[11].offset_mid = (uint16)(((uint64)&isr11 & 0x00000000ffff0000) >> 16);
	_idt[11].offset_high = (uint32)(((uint64)&isr11 & 0xffffffff00000000) >> 32);
	_idt[11].ist = 0;
	_idt[11].selector = 0x08;
	_idt[11].types_attr = 0x8e;

	_idt[12].zero = 0;
	_idt[12].offset_low = (uint16)(((uint64)&isr12 & 0x000000000000ffff));
	_idt[12].offset_mid = (uint16)(((uint64)&isr12 & 0x00000000ffff0000) >> 16);
	_idt[12].offset_high = (uint32)(((uint64)&isr12 & 0xffffffff00000000) >> 32);
	_idt[12].ist = 0;
	_idt[12].selector = 0x08;
	_idt[12].types_attr = 0x8e;

	_idt[13].zero = 0;
	_idt[13].offset_low = (uint16)(((uint64)&isr13 & 0x000000000000ffff));
	_idt[13].offset_mid = (uint16)(((uint64)&isr13 & 0x00000000ffff0000) >> 16);
	_idt[13].offset_high = (uint32)(((uint64)&isr13 & 0xffffffff00000000) >> 32);
	_idt[13].ist = 0;
	_idt[13].selector = 0x08;
	_idt[13].types_attr = 0x8e;

	_idt[14].zero = 0;
	_idt[14].offset_low = (uint16)(((uint64)&isr14 & 0x000000000000ffff));
	_idt[14].offset_mid = (uint16)(((uint64)&isr14 & 0x00000000ffff0000) >> 16);
	_idt[14].offset_high = (uint32)(((uint64)&isr14 & 0xffffffff00000000) >> 32);
	_idt[14].ist = 0;
	_idt[14].selector = 0x08;
	_idt[14].types_attr = 0x8e;

	_idt[16].zero = 0;
	_idt[16].offset_low = (uint16)(((uint64)&isr16 & 0x000000000000ffff));
	_idt[16].offset_mid = (uint16)(((uint64)&isr16 & 0x00000000ffff0000) >> 16);
	_idt[16].offset_high = (uint32)(((uint64)&isr16 & 0xffffffff00000000) >> 32);
	_idt[16].ist = 0;
	_idt[16].selector = 0x08;
	_idt[16].types_attr = 0x8e;

	_idt[17].zero = 0;
	_idt[17].offset_low = (uint16)(((uint64)&isr17 & 0x000000000000ffff));
	_idt[17].offset_mid = (uint16)(((uint64)&isr17 & 0x00000000ffff0000) >> 16);
	_idt[17].offset_high = (uint32)(((uint64)&isr17 & 0xffffffff00000000) >> 32);
	_idt[17].ist = 0;
	_idt[17].selector = 0x08;
	_idt[17].types_attr = 0x8e;

	_idt[18].zero = 0;
	_idt[18].offset_low = (uint16)(((uint64)&isr18 & 0x000000000000ffff));
	_idt[18].offset_mid = (uint16)(((uint64)&isr18 & 0x00000000ffff0000) >> 16);
	_idt[18].offset_high = (uint32)(((uint64)&isr18 & 0xffffffff00000000) >> 32);
	_idt[18].ist = 0;
	_idt[18].selector = 0x08;
	_idt[18].types_attr = 0x8e;

	_idt[19].zero = 0;
	_idt[19].offset_low = (uint16)(((uint64)&isr19 & 0x000000000000ffff));
	_idt[19].offset_mid = (uint16)(((uint64)&isr19 & 0x00000000ffff0000) >> 16);
	_idt[19].offset_high = (uint32)(((uint64)&isr19 & 0xffffffff00000000) >> 32);
	_idt[19].ist = 0;
	_idt[19].selector = 0x08;
	_idt[19].types_attr = 0x8e;

	RemapPIC();
	SetMaskPIC1(0b11111101);
	SetMaskPIC2(0b11111111);

	LoadIDT();
}

extern "C" void isr0_handler()
{
	uint8 irq0 = (inb(PIC1_DATA) & 0b00000001);
	uint8 irq1 = (inb(PIC1_DATA) & 0b00000010) >> 1;

	if (!irq0)
	{
		irq0_count++;

		SendEOI(0);

		return;
	}

	if (!irq1)
	{
		if (cursorPos % 80 != 0)
			Print("\r\n");

		Print("Error:", lightredF); Print(" Division by zero.");

		SendEOI(0);

		asm("hlt");
	}
}

extern "C" void isr1_handler()
{
	uint8 scanCode = inb(0x60);
	uint8 chr = 0;

	if (scanCode < 0x3a)
		chr = ScanCodeLookupTable[scanCode];
	if (scanCode == 0x56)
		chr = ScanCodeLookupTable[0x2b];

	if (MainKeyboardHandler != 0)
		MainKeyboardHandler(scanCode, chr);

	SendEOI(1);
}

extern "C" void isr3_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Breakpoint - trap.");

	SendEOI(3);

	asm("hlt");
}

extern "C" void isr4_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Overflow.");

	SendEOI(4);

	asm("hlt");
}

extern "C" void isr5_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" BOUND Range Exceeded.");

	SendEOI(5);

	asm("hlt");
}

extern "C" void isr6_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Invalid Opcode.");

	SendEOI(6);

	asm("hlt");
}

extern "C" void isr7_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Device Not Available (No Math Coprocessor).");

	SendEOI(7);

	asm("hlt");
}

extern "C" void isr8_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Double Fault.");

	SendEOI(8);

	asm("hlt");
}

extern "C" void isr9_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Coprocessor Segment Overrun (reserved).");

	SendEOI(9);

	asm("hlt");
}

extern "C" void isr10_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Invalid TSS.");

	SendEOI(10);

	asm("hlt");
}

extern "C" void isr11_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Segment Not Present.");

	SendEOI(11);

	asm("hlt");
}

extern "C" void isr12_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Stack-Segment Fault.");

	SendEOI(12);

	asm("hlt");
}

extern "C" void isr13_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" General Protection Fault.");

	SendEOI(13);

	asm("hlt");
}

extern "C" void isr14_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Page Fault.");

	SendEOI(14);

	asm("hlt");
}

extern "C" void isr16_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" x87 FPU Floating-Point Error (Math Fault).");

	SendEOI(16);

	asm("hlt");
}

extern "C" void isr17_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Alignment Check.");

	SendEOI(17);

	asm("hlt");
}

extern "C" void isr18_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" Machine Check.");

	SendEOI(18);

	asm("hlt");
}

extern "C" void isr19_handler(void) {

	if (cursorPos % 80 != 0)
		Print("\r\n");

	Print("Error:", lightredF); Print(" SIMD Floating-Point Exception.");

	SendEOI(19);

	asm("hlt");
}