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
extern uint64 isr60;
extern uint64 isr61;

extern "C" void LoadIDT();

void (*MainKeyboardHandler)(uint8 scanCode, uint8 chr);

void SetIDT(uint64 isr, uint8 num)
{
	_idt[num].zero = 0;
	_idt[num].offset_low = (uint16)(isr & 0x000000000000ffff);
	_idt[num].offset_mid = (uint16)((isr & 0x00000000ffff0000) >> 16);
	_idt[num].offset_high = (uint32)((isr & 0xffffffff00000000) >> 32);
	_idt[num].ist = 0;
	_idt[num].selector = 0x08;
	_idt[num].types_attr = 0x8e;
}

void InitializeIDT()
{
	SetIDT((uint64)&isr0, 0);
	SetIDT((uint64)&isr1, 1);
	SetIDT((uint64)&isr3, 3);
	SetIDT((uint64)&isr4, 4);
	SetIDT((uint64)&isr5, 5);
	SetIDT((uint64)&isr6, 6);
	SetIDT((uint64)&isr7, 7);
	SetIDT((uint64)&isr8, 8);
	SetIDT((uint64)&isr9, 9);
	SetIDT((uint64)&isr10, 10);
	SetIDT((uint64)&isr11, 11);
	SetIDT((uint64)&isr12, 12);
	SetIDT((uint64)&isr13, 13);
	SetIDT((uint64)&isr14, 14);
	SetIDT((uint64)&isr16, 16);
	SetIDT((uint64)&isr17, 17);
	SetIDT((uint64)&isr18, 18);
	SetIDT((uint64)&isr19, 19);

	SetIDT((uint64)&isr60, 60);
	SetIDT((uint64)&isr61, 61);

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

uint8 ind = 0;

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

extern "C" void isr60_handler(void) {

	syscall_vga();
	SendEOI(60);	
}

extern "C" void isr61_handler(void) {

	
	SendEOI(61);
}