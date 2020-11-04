#pragma once
#include "Typedefs.h"
#include "VGA.h"
#include "../Heap.h"
#include "../Utils.h"

const char ScanCodeLookupTable[] =
{
    0, 0, '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=', 0, 0,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
    0, 0, 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/', 0, '*',
    0, ' '
};

extern char* msg;
extern uint16 cmdPos;
extern uint16 selectStart;
extern uint16 selectEnd;

extern bool LeftShiftPressed;
extern bool RightShiftPressed;
extern bool caps;
extern bool ctrl;
extern bool selectMode;

extern uint8 lastScanCode;

extern void (*EnterHandler)();
extern void ClearMessage();
extern void KeyboardHandler(uint8 scanCode, uint8 chr);
extern void DeleteSelection();