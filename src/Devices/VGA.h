#pragma once
#include "../IO.h"
#include "../Typedefs.h"

#define blackF 0x00
#define blueF 0X01
#define greenF 0x02
#define cyanF 0X03
#define redF 0x04
#define magentaF 0x05
#define brownF 0x06
#define lightgrayF 0x07
#define darkgrayF 0x08
#define lightblueF 0X09
#define lightgreenF 0x0a
#define lightcyanF 0X0b
#define lightredF 0x0c
#define lightmagentaF 0X0d
#define yellowF 0x0e
#define whiteF 0x0f

#define blackB 0x00
#define blueB 0X10
#define greenB 0x20
#define cyanB 0X30
#define redB 0x40
#define magentaB 0x50
#define brownB 0x60
#define lightgrayB 0x70
#define blinkblackB 0x80
#define blinkblueB 0x90
#define blinkgreenB 0xa0
#define blinkcyanB 0xb0
#define blinkredB 0xc0
#define blinkmagentaB 0xd0
#define blinkyellowB 0xe0
#define blinkwhiteB 0xf0

#define VGA_MEMORY (uint8*) 0xb8000
#define VGA_WIDTH 80

extern uint16 cursorPos;

void SetCursorPosition(uint16 pos);
void SetCursorPosition(uint8 x, uint8 y);
void Scroll(uint64 color = whiteF);
void ClearScreen(uint64 color = whiteF);
void ClearScreen(uint16 start, uint16 length);
void PaintScreen(uint16 startPos, uint16 length, uint64 color = whiteF);
void Print(const char* str, uint8 color = whiteF);
void PrintBack(const char* str, uint8 color = whiteF);
void Print(uint8 value, uint8 color = whiteF, bool moveCursor = true);