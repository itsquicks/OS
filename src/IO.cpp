#include "IO.h"

void outb(uint16 port, uint8 val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8 inb(uint16 port)
{
	uint8 ret;
	asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

void outw(uint16 port, uint16 val)
{
	asm volatile ("outw %0, %1" : : "a"(val), "d"(port));
}

uint8 inw(uint16 port)
{
	uint16 ret;
	asm volatile ("inw %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}