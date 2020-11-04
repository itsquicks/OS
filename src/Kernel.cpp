#include "IDT.h"
#include "Heap.h"
#include "Memory.h"
#include "Time.h"
#include "Utils.h"
#include "Filesystem.h"
#include "Devices/CPU.h"
#include "Devices/VGA.h"
#include "Devices/Keyboard.h"
#include "Devices/ATA.h"

//extern "C" void tripleFault();

void EnterPressed()
{
	uint8 argc = wordcount(msg);
	char** argv = splitwords(msg, argc);

	if (argc > 0)
	{
		if (strcmp(argv[0], "cls") && argc == 1)
		{
			ClearScreen();
			SetCursorPosition(0);
		}
		else if (strcmp(argv[0], "cpu") && argc == 1)
		{
			Get_CPU_Name();
			Print("CPU name: ", lightcyanF); Print((char*)cpu_name);
			Print("\r\n");
			Get_CPU_Speed();
			Print("Speed: ", lightcyanF);  Print(IntToString(cpu_speed)); Print(" MHz");
			Print("\r\n");
		}
		else if (strcmp(argv[0], "mem") && argc == 1)
		{
			PrintMemorySize();
			Print("\r\n");
		}
		else if (strcmp(argv[0], "memdmp") && argc == 3)
		{
			uint64 address = StringToHex(argv[1]);
			uint8 rows = StringToInt(argv[2]);
			PrintMemoryDump(address, rows);
		}
		else if (strcmp(argv[0], "mbr") && argc == 1)
		{
			PrintMBR(mbr);
		}
		else if (strcmp(argv[0], "bpb") && argc == 1)
		{
			PrintBPB(bpb);
		}
		else if (strcmp(argv[0], "ls") && argc == 1)
		{
			PrintDirectoryContent();
		}
		else if (strcmp(argv[0], "test"))
		{
			Print(IntToString(argc));
			Print("\r\n");
			for (uint8 i = 0; i < argc; i++)
			{
				Print(argv[i]);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "show") && argc == 2)
		{
			DirectoryEntry* file = (DirectoryEntry*)DirectoryFromName(argv[1]);

			if (file!=0)
			{
				if ((file->attributes & 0x10) == 0x10)
				{
					Print("Cannot use 'show' for directories.", lightgrayF);
					Print("\r\n");
				}
				else if (file->size == 0)
				{
					Print("File is empty.", lightgrayF);
					Print("\r\n");
				}
				else
				{
					for (uint8 i = 0; i < 80; i++)
						Print(196, darkgrayF);

					PrintFileContent(file);
					Print("\r\n");

					for (uint8 i = 0; i < 80; i++)
						Print(196, darkgrayF);
				}
			}
			else
			{
				Print("File not found.", lightgrayF);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "cd") && argc == 2)
		{
		     DirectoryEntry* dir = (DirectoryEntry*)DirectoryFromName(argv[1]);

			 if ((dir->attributes & 0x10) == 0x10 && dir != 0)
			 {
				 ChangeDirectory(dir);
				 char* name = GetFileName(dir);

				 if (strcmp(argv[1], ".."))
				 {
					 uint8 i = strlen(path);
					 while (*(path + i) != '/')
					 {
						 *(path + i) = 0;
						 i--;
					 }
					 *(path + i) = 0;
				 }
				 else if (!strcmp(argv[1],"."))
				 {
					 stradd(path, '/');
					 stradd(path, name);
				 }
			 }
			 else
			 {
				 Print("Directory not found.", lightgrayF);
				 Print("\r\n");
			 }
		}
		else if (strcmp(argv[0], "del") && argc == 2)
		{
			DirectoryEntry* dir = (DirectoryEntry*)DirectoryFromName(argv[1]);

			if (dir != 0)
			{
				DeleteFile(dir);
			}
			else
			{
				Print("Directory not found.", lightgrayF);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "wd") && argc == 4)
		{
			uint32 sector = StringToInt(argv[1]);
			uint8* data = (uint8*)StringToHex(argv[2]);
			uint16 count = StringToInt(argv[3]);

			ATA_Write(hd, sector, data, count);
			ATA_Flush(hd);
		}
		else if (strcmp(argv[0], "rd") && argc == 4)
		{
			uint32 sector = StringToInt(argv[1]);
			uint8* data = (uint8*)StringToHex(argv[2]);
			uint16 count = StringToInt(argv[3]);

			ATA_Read(hd, sector, data, count);
		}
		else if (strcmp(argv[0], "outb") && argc == 3)
		{
			uint16 port = StringToHex(argv[1]);
			uint8 value = StringToHex(argv[2]);
			outb(port, value);
		}
		else if (strcmp(argv[0], "inb") && argc == 2)
		{
			uint16 port = StringToHex(argv[1]);
			uint8 value = inb(port);
			Print(HexToString(value), lightcyanF);
			Print("\r\n");
		}
		else if (strcmp(argv[0], "time") && argc == 1)
		{
			//Read_RTC();
			PrintTime();
			Print("\r\n");
		}
		else if (strcmp(argv[0], "date") && argc == 1)
		{
			//Read_RTC();
			PrintDate();
			Print("\r\n");
		}
		else if (strcmp(argv[0], "sleep") && argc == 2)
		{
			uint64 ms = StringToInt(argv[1]);
			//Sleep(ms);
		}
		else if (strcmp(argv[0], "restart") && argc == 1)
		{
			//tripleFault();
		}
		else
		{
			Print("Unknown command.", lightgrayF);
			Print("\r\n");
		}
	}

	for (uint8 i = 0; i < argc; i++)
		free(argv[i]);

	ClearMessage();
	
	uint8 i = 0;
	while (*(path + i) != 0)
	{
		if (*(path + i) == '/')
			Print(*(path + i), lightredF);
		else
			Print(*(path + i), lightblueF);
		i++;
	}

	Print('>', lightredF);
	cmdPos = cursorPos-1;
}

extern "C" void _start()
{
	InitializeIDT();
	MemoryMapEntry** UsableMemoryMaps = GetUsableMemoryRegions();
	InitializeHeap(0x100000, 0x100000);
	InitializeFilesystem();

	MainKeyboardHandler = KeyboardHandler;
	EnterHandler = EnterPressed;

	ClearScreen();
	ClearMessage();
	SetCursorPosition(0);

	uint8 i = 0;
	while (*(path + i) != 0)
	{
		if (*(path + i) == '/')
			Print(*(path + i), lightredF);
		else
			Print(*(path + i), lightblueF);
		i++;
	}
	Print('>', lightredF);
	cmdPos = cursorPos-1;

	return;
}