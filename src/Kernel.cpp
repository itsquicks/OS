//#include "IDT.h"
#include "Heap.h"
//#include "Memory.h"
//#include "Time.h"
//#include "Utils.h"
//#include "Filesystem.h"
//#include "Devices/CPU.h"
//#include "Devices/VGA.h"
//#include "Devices/Keyboard.h"
//#include "Devices/ATA.h"

/*extern "C" void tripleFault();

bool startProgram = false;
DirectoryEntry* programInfo = 0;

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
		else if (strcmp(argv[0], "debug") && argc == 1)
		{
			debug = !debug;
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
		else if (strcmp(argv[0], "setbytes") && argc >= 3)
		{	
			uint8* ptr = (uint8*)StringToHex(argv[1]);
			for (uint8 i = 2; i < argc; i++)
			{
				if (strcmp(argv[i], "~"))
				{
					ptr++;
					continue;
				}

				uint8 val = (uint8)StringToHex(argv[i]);				
				*ptr = val; 
				ptr++;
			}
		}
		else if (strcmp(argv[0], "run") && argc == 2)
		{
			uint64* address = (uint64*)StringToHex(argv[1]);
			
			void (*main) ();
			main = (void (*)())address;

			asm("pushq %rax");
			asm("pushq %rcx");
			asm("pushq %rdx");
			asm("pushq %rbx");
			asm("pushq %rsi");
			asm("pushq %rdi");

			main();

			asm("popq %rax");
			asm("popq %rcx");
			asm("popq %rdx");
			asm("popq %rbx");
			asm("popq %rsi");
			asm("popq %rdi");
		}
		else if (strcmp(argv[0], "mbr") && argc == 1)
		{
			PrintMBR(mbr);
		}
		else if (strcmp(argv[0], "bpb") && argc == 1)
		{
			PrintBPB(bpb);
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
		else if (strcmp(argv[0], "ls") && argc == 1)
		{
			PrintDirectoryContent();
		}
		else if (strcmp(argv[0], "show") && argc == 2)
		{
			DirectoryEntry* file = DirectoryFromName(argv[1]);

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

					PrintFileContent(*file);
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
		else if (strcmp(argv[0], "load") && argc == 3)
		{
			DirectoryEntry* file = DirectoryFromName(argv[1]);
			uint64 address = StringToHex(argv[2]);

			if (file != 0)
			{
				if ((file->attributes & 0x10) == 0x10)
				{
					Print("Cannot use 'load' for directories.", lightgrayF);
					Print("\r\n");
				}
				else if (file->size == 0)
				{
					Print("File is empty.", lightgrayF);
					Print("\r\n");
				}
				else
					LoadFile(*file,address);
			}
			else
			{
				Print("File not found.", lightgrayF);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "save") && argc == 4)
		{
			DirectoryEntry* file = DirectoryFromName(argv[1]);
			uint64 address = StringToHex(argv[2]);
			uint32 newSize = (uint32)StringToInt(argv[3]);

			if (file != 0)
			{
				if ((file->attributes & 0x10) == 0x10)
				{
					Print("Cannot use 'save' for directories.", lightgrayF);
					Print("\r\n");
				}
				else
					SaveFile(*file, address, newSize);
			}
			else
			{
				Print("File not found.", lightgrayF);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "ren") && argc == 3)
		{
			DirectoryEntry* file = DirectoryFromName(argv[1]);

			if (file != 0)
			{
				RenameFile(*file, argv[2]);
			}
			else
			{
				Print("Directory / File not found.", lightgrayF);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "cd") && argc == 2)
		{
		    DirectoryEntry* dir = DirectoryFromName(argv[1]);

			 if ((dir->attributes & 0x10) == 0x10 && dir != 0)
			 {				 			 
				 char* name = GetFileName(*dir);
				 ChangeDirectory(*dir);

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
			DirectoryEntry* dir = DirectoryFromName(argv[1]);

			if (dir != 0)
			{
				if ((dir->attributes & 0x10) == 0x10)
				{
					if (dir->name[0] == 0x2E)
					{
						Print("Cannot delete directory.", lightgrayF);
						Print("\r\n");
					}
					else
						DeleteDirectory(*dir);
				}
				else
					DeleteFile(*dir);
			}
			else
			{
				Print("Directory / File not found.", lightgrayF);
				Print("\r\n");
			}
		}
		else if (strcmp(argv[0], "new") && argc >= 2)
		{
		    uint8 attributes = 0;

			for (uint8 i = 2; i < argc; i++)
			{
				if (strcmp(argv[i], "-d"))
					attributes |= 0x10;
				else if (strcmp(argv[i], "-h"))
					attributes |= 0x02;
				else if (strcmp(argv[i], "-r"))
					attributes |= 0x01;
				else if (strcmp(argv[i], "-s"))
					attributes |= 0x04;
			}

			NewFile(argv[1], attributes);
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
			Read_RTC();
			PrintTime();
			Print("\r\n");
		}
		else if (strcmp(argv[0], "date") && argc == 1)
		{
			Read_RTC();
			PrintDate();
			Print("\r\n");
		}
		else if (strcmp(argv[0], "sleep") && argc == 2)
		{
			uint64 ms = StringToInt(argv[1]);
			Sleep(ms);
		}
		else if (strcmp(argv[0], "restart") && argc == 1)
		{
			tripleFault();
		}
		else
		{
		    char* actualName = (char*)calloc(32); 
			strcpy(actualName, argv[0]);
			stradd(actualName, ".exe");

			DirectoryEntry* programEntry = DirectoryFromName(actualName);
			free(actualName);

			if (programEntry == 0)
			{
				Print("Unknown command.\r\n", lightgrayF);
			}
			else
			{
				if ((programEntry->attributes & 0x10) == 0x10)
				{
					Print("Cannot execute directory.\r\n", lightgrayF);
				}
				else
				{
					if (programEntry->size == 0)
					{
						Print("Cannot execute empty file.\r\n", lightgrayF);
					}
					else
					{
						startProgram = true;
						programInfo = programEntry;
					}
				}
			}
		}
	}

	for (uint8 i = 0; i < argc; i++)
		free(argv[i]);
	free(argv);

	ClearMessage();
	
	if (startProgram)
		return;

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
}*/

extern "C" void _start()
{
	/*InitializeIDT();
	MemoryMapEntry** UsableMemoryMaps = GetUsableMemoryRegions();
	InitializeHeap(0x100000, 0x100000);
	InitializeFilesystem();

	InitializeCPUID();
	InitializeInput();
	MainKeyboardHandler = KeyboardHandler;
	EnterHandler = EnterPressed;

	ClearScreen();
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

	while (1)
	{
		if (startProgram)
		{
			void* program = calloc(programInfo->size);
			LoadFile(*programInfo, (uint64)program);

			void (*main) ();
			main = (void (*)())(uint64)program;

			asm volatile("pushq %rax");
			asm volatile("pushq %rcx");
			asm volatile("pushq %rdx");
			asm volatile("pushq %rbx");
			asm volatile("pushq %rsi");
			asm volatile("pushq %rdi");

			main();

			asm volatile("popq %rdi");
			asm volatile("popq %rsi");
			asm volatile("popq %rbx");
			asm volatile("popq %rdx");
			asm volatile("popq %rcx");
			asm volatile("popq %rax");

			free(program);
			startProgram = false;
		}
	}

	return;*/

	debug = true;
	InitializeHeap(0x100000, 0x100000);
	ClearScreen(0x1f);
	

	void* ptr = calloc(1024);
	ptr = realloc(ptr, 2048);

	for (uint8 i = 0; i < 7; i++)
		calloc(256) - 1;
}