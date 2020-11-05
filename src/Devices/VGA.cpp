#include "VGA.h"

uint16 cursorPos;

void SetCursorPosition(uint16 pos)
{
	if (pos < 2000)
	{
		outb(0x3d4, 0x0f);
		outb(0x3d5, (uint8)(pos & 0xff));
		outb(0x3d4, 0x0e);
		outb(0x3d5, (uint8)((pos >> 8) & 0xff));

		cursorPos = pos;
	}
	else
	{
		Scroll();
		SetCursorPosition(pos - VGA_WIDTH);
	}
}

void SetCursorPosition(uint8 x, uint8 y)
{
	uint16 pos = y * VGA_WIDTH + x;
	SetCursorPosition(pos);
}

void Scroll(uint64 color)
{
	uint64* up = (uint64*)(VGA_MEMORY);
	uint64* down = (uint64*)(VGA_MEMORY + VGA_WIDTH * 2);

	for (uint16 i = 0; i < 24 * 20; i++)
	{
		*up = *down;
		up++;
		down++;
	}

	uint64 value = 0;
	value += color << 8;
	value += color << 24;
	value += color << 40;
	value += color << 56;

	for (uint8 i = 0; i < 20; i++)
	{
		*up = value;
		up++;
	}
}

void ClearScreen(uint64 color)
{
	uint64 value = 0;
	value += color << 8;
	value += color << 24;
	value += color << 40;
	value += color << 56;

	for (uint64* i = (uint64*)VGA_MEMORY; i < (uint64*)(VGA_MEMORY + 4000); i++)
	{
		*i = value;
	}
}

void ClearScreen(uint16 start, uint16 length, uint16 color)
{
	uint16 value = 0;
	value += color << 8;

	for (uint16* i = (uint16*)(VGA_MEMORY + 2 * start); i < (uint16*)(VGA_MEMORY + 2 * (length + start)); i++)
	{
		*i = value;
	}
}

void PaintScreen(uint16 start, uint16 length, uint16 color)
{
	for (uint16* i = (uint16*)(VGA_MEMORY + 2 * start); i < (uint16*)(VGA_MEMORY + 2 * (length + start)); i++)
	{
		uint16 value = *i;
		value &= 0x00ff;
		value += color << 8;
		*i = value;
	}
}

void Print(const char* str, uint8 color)
{
	uint8* charPtr = (uint8*)str;

	while (*charPtr != 0)
	{
		switch (*charPtr)
		{
		case 10:    //new line case
			SetCursorPosition(cursorPos + VGA_WIDTH);
			break;
		case 13:    //return case
			SetCursorPosition(cursorPos - (cursorPos % VGA_WIDTH));
			break;
		default:
			Print(*charPtr, color);
		}

		charPtr++;
	}
}

void PrintBack(const char* str, uint8 color)
{
	uint8 len = strlen(str);
	if (cursorPos >= len)
		SetCursorPosition(cursorPos - len);

	uint8* charPtr = (uint8*)str;

	while (*charPtr != 0)
	{
		switch (*charPtr)
		{
		case 10:    //new line case
			SetCursorPosition(cursorPos + VGA_WIDTH);
			break;
		case 13:    //return case
			SetCursorPosition(cursorPos - (cursorPos % VGA_WIDTH));
			break;
		default:
			Print(*charPtr, color);
		}

		charPtr++;
	}
}

void Print(uint8 value, uint8 color, bool moveCursor)
{
	*(VGA_MEMORY + cursorPos * 2) = value;
	*(VGA_MEMORY + cursorPos * 2 + 1) = color;

	if (moveCursor)
		SetCursorPosition(cursorPos + 1);
}