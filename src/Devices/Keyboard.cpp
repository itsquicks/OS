#include "Keyboard.h"

char* msg;
char** history;
uint8 i;
uint8 h;
uint8 count;
uint16 cmdPos;
uint16 selectStart;
uint16 selectEnd;

bool LeftShiftPressed = false;
bool RightShiftPressed = false;
bool caps = false;
bool ctrl = false;
bool selectMode = false;

uint8 lastScanCode;

void (*EnterHandler)();

void InitializeInput()
{
	msg = (char*)calloc(256);

	history = (char**)calloc(5 * sizeof(char*));
	for (uint8 j = 0; j < 5; j++)
		history[j] = (char*)calloc(256);

	i = 0;
	h = 0;
	count = 0;
	
	cmdPos = 0;
	selectStart = 0;
	selectEnd = 0;
}

void ClearMessage()
{
	if (*msg != 0)
	{
		for (uint8 j = 4; j > 0; j--)
		{
			strcpy(history[j], history[j - 1]);
		}
		strcpy(history[0], msg);

		if (count < 5)
			count++;
	}

	*msg = 0;
	i = 0;
	h = 0;
}

void KeyboardHandler(uint8 scanCode, uint8 chr)
{
	uint8 icpy = i;

	if (ctrl && chr == 'a')
	{
		selectMode = true;
		selectStart = cmdPos + 1;
		selectEnd = cmdPos + i + 2;
	}
	else
	{
		if (LeftShiftPressed || RightShiftPressed || caps)
		{
			if (chr >= 'a' && chr <= 'z')
			{
				chr -= 32;
			}
			else if (LeftShiftPressed || RightShiftPressed)
			{
				switch (chr)
				{
				case '`':
					chr = '~';
					break;
				case '1':
					chr = '!';
					break;
				case '2':
					chr = '@';
					break;
				case '3':
					chr = '#';
					break;
				case '4':
					chr = '$';
					break;
				case '5':
					chr = '%';
					break;
				case '6':
					chr = '^';
					break;
				case '7':
					chr = '&';
					break;
				case '8':
					chr = '*';
					break;
				case '9':
					chr = '(';
					break;
				case '0':
					chr = ')';
					break;
				case '-':
					chr = '_';
					break;
				case '=':
					chr = '+';
					break;
				case '[':
					chr = '{';
					break;
				case ']':
					chr = '}';
					break;
				case ';':
					chr = ':';
					break;
				case 0x27:
					chr = 0x22;
					break;
				case 0x5c:
					chr = 0x7c;
					break;
				case ',':
					chr = '<';
					break;
				case '.':
					chr = '>';
					break;
				case '/':
					chr = '?';
					break;
				}
			}
		}

		if (chr != 0)
		{
			if (selectMode)
				DeleteSelection();

			if (i < 254)
			{
				shiftup(msg, cursorPos - cmdPos - 1, chr);
				if (cursorPos + 1 > 1999)
				{
					cmdPos -= VGA_WIDTH;
				}
				SetCursorPosition(cursorPos + 1);
				i++;
			}
		}
		else
		{
			switch (scanCode)
			{
			case 0x0e:                                 //Backspace
				if (selectMode)
				{
					DeleteSelection();
				}
				else
				{
					if (cursorPos - 1 > cmdPos)
					{
						shiftdown(msg, cursorPos - cmdPos - 2);
						SetCursorPosition(cursorPos - 1);
						i--;
					}
				}
				break;
			case 0x2a:                                 //Left Shift Pressed
				LeftShiftPressed = true;
				break;
			case 0xaa:                                 //Left Shift Released
				LeftShiftPressed = false;
				break;
			case 0x36:                                 //Right Shift Pressed
				RightShiftPressed = true;
				break;
			case 0xb6:                                 //Right Shift Released
				RightShiftPressed = false;
				break;
			case 0x1c:                                 //Enter 
				selectMode = false;
				PaintScreen(selectStart, selectEnd - selectStart, whiteF);
				Print("\r\n");
				EnterHandler();
				return;
				break;
			case 0x48:                                 //Up arrow
				if (h + 1 <= count)
				{
					h += 1;
					selectMode = false;
					strcpy(msg, history[h - 1]);
					i = strlen(msg);
					SetCursorPosition(cmdPos + i + 1);
				}
				break;
			case 0x50:                                 //Down arrow
				if (h > 1)
				{
				    h--;
					selectMode = false;
					strcpy(msg, history[h - 1]);
					i = strlen(msg);
					SetCursorPosition(cmdPos + i + 1);
				}
				break;
			case 0x4b:                                 //Left arrow
				if ((LeftShiftPressed || RightShiftPressed) && !selectMode)
				{
					selectMode = true;
					selectStart = cursorPos;
					selectEnd = cursorPos + 1;
					break;
				}
				if (cursorPos - 1 > cmdPos)
				{
					if (LeftShiftPressed || RightShiftPressed)
					{
						if (cursorPos == selectStart)
							selectStart--;
					}
					else
					{
						selectMode = false;
					}
					SetCursorPosition(cursorPos - 1);
				}
				break;
			case 0x4d:                                 //Right arrow
				if ((LeftShiftPressed || RightShiftPressed) && !selectMode)
				{
					selectMode = true;
					selectStart = cursorPos;
					selectEnd = cursorPos + 1;
					break;
				}
				if (cursorPos - 1 < cmdPos + i)
				{
					if (LeftShiftPressed || RightShiftPressed)
					{
						if (cursorPos + 1 == selectEnd)
							selectEnd++;
					}
					else
					{
						selectMode = false;
					}
					SetCursorPosition(cursorPos + 1);
				}
				break;
			case 0x3a:                                 //Caps Lock
				caps = !caps;
				break;
			case 0x1d:                                 //Control down
				ctrl = true;
				break;
			case 0x9d:                                 //Control up
				ctrl = false;
				break;
			}
		}
	}

	lastScanCode = scanCode;

	uint16 lastPos = cursorPos;
	SetCursorPosition(cmdPos + 1);
	ClearScreen(cmdPos + 1, icpy + 1);
	Print(msg);
	if (selectMode)
		PaintScreen(selectStart, selectEnd - selectStart, blueB | lightcyanF);
	SetCursorPosition(lastPos);
}

void DeleteSelection()
{
	SetCursorPosition(selectStart);
	while (selectEnd - selectStart != 0)
	{
		if (msg[selectStart - cmdPos - 1] != 0)
		{
			shiftdown(msg, selectStart - cmdPos - 1);
			i--;
		}
		selectEnd--;
	}
	selectMode = false;
}