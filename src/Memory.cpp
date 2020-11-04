#include "Memory.h"

uint8 UsableMemoryRegionCount;
MemoryMapEntry* UsableMemoryRegions[10];

uint64 memSize;
uint64 usableMemSize;

void PrintMemoryMap(MemoryMapEntry* memoryMap)
{
	Print("Memory Base: ", lightgreenF); Print(IntToString(memoryMap->BaseAdress));
	Print("\r\nRegion Length: ", lightgreenF); Print(IntToString(memoryMap->RegionLength));
	Print("\r\nMemory Type: ", lightgreenF); Print(IntToString(memoryMap->RegionType));
	Print("\r\nMemory Attributes: ", lightgreenF); Print(HexToString(memoryMap->ExtendedAttributes));
}

bool MemoryRegionsGot = false;

MemoryMapEntry** GetUsableMemoryRegions()
{
	if (MemoryRegionsGot)
		return UsableMemoryRegions;

	memSize = 0;
	usableMemSize = 0;

	uint8 UsableMemoryIndex;
	for (uint8 i = 0; i < MemoryRegionCount; i++)
	{
		MemoryMapEntry* memMap = (MemoryMapEntry*)0x5000;
		memMap += i;
		memSize += memMap->RegionLength;

		if (memMap->RegionType == 1)
		{
			UsableMemoryRegions[UsableMemoryIndex] = memMap;
			usableMemSize += memMap->RegionLength;
			UsableMemoryIndex++;
		}
	}

	UsableMemoryRegionCount = UsableMemoryIndex;

	MemoryRegionsGot = true;
	return UsableMemoryRegions;
}

void PrintMemorySize()
{
	float p = usableMemSize / (float)memSize;
	p *= 100;

	Print("Memory size: ", lightgreenF); Print(IntToString(memSize)); Print(" bytes.");
	Print("\r\n");
	Print("Usable memory size: ", lightgreenF); Print(IntToString(usableMemSize));  Print(" bytes.   ");
	Print(FloatToString(p, 2), yellowF); Print('%', yellowF);
}

void PrintMemoryDump(uint64 address, uint8 rows)
{
	uint8* ptr = (uint8*)address;

	for (uint8 i = 0; i < rows; i++)
	{
		Print(HexToString((uint64)ptr), lightgreenF); Print("   ");
		for (uint8 n = 0; n < 16; n++)
		{
			Print(HexToString(*ptr)); Print(' ');
			ptr++;
		}
		Print("\r\n");
	}
}

void memset(void* start, uint64 value, uint64 num)
{
	if (num <= 8)
	{
		uint8* valPtr = (uint8*)&value;
		for (uint8* ptr = (uint8*)start; ptr < (uint8*)((uint64)start + num); ptr++)
		{
			*ptr = *valPtr;
			valPtr++;
		}

		return;
	}

	uint64 procBytes = num % 8;
	uint64 newnum = num - procBytes;

	for (uint64* ptr = (uint64*)start; ptr < (uint64*)((uint64)start + newnum); ptr++)
	{
		*ptr = value;
	}

	uint8* valPtr = (uint8*)&value;
	for (uint8* ptr = (uint8*)((uint64)start + newnum); ptr < (uint8*)((uint64)start + num); ptr++)
	{
		*ptr = *valPtr;
		valPtr++;
	}
}

void memcpy(void* destination, void* src, uint64 num)
{
	if (num <= 8)
	{
		uint8* valPtr = (uint8*)src;
		for (uint8* ptr = (uint8*)destination; ptr < (uint8*)((uint64)destination + num); ptr++)
		{
			*ptr = *valPtr;
			valPtr++;
		}

		return;
	}

	uint64 procBytes = num % 8;
	uint64 newnum = num - procBytes;
	uint64* srcPtr = (uint64*)src;

	for (uint64* destPtr = (uint64*)destination; destPtr < (uint64*)((uint64)destination + newnum); destPtr++)
	{
		*destPtr = *srcPtr;
		srcPtr++;
	}

	uint8* srcPtr8 = (uint8*)((uint64)src + newnum);
	for (uint8* destPtr8 = (uint8*)((uint64)destination + newnum); destPtr8 < (uint8*)((uint64)destination + num); destPtr8++)
	{
		*destPtr8 = *srcPtr8;
		srcPtr8++;
	}
}