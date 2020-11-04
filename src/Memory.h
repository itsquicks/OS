#pragma once
#include "Typedefs.h"
#include "Utils.h"
#include "Devices/VGA.h"

struct MemoryMapEntry
{
	uint64 BaseAdress;
	uint64 RegionLength;
	uint32 RegionType;
	uint32 ExtendedAttributes;
};

extern uint8 MemoryRegionCount;
extern uint8 UsableMemoryRegionCount;

extern uint64 memSize;
extern uint64 usableMemSize;

MemoryMapEntry** GetUsableMemoryRegions();
void PrintMemoryMap(MemoryMapEntry* memoryMap);
void PrintMemorySize();
void PrintMemoryDump(uint64 address, uint8 rows);

void memset(void* start, uint64 value, uint64 num);
void memcpy(void* destination, void* src, uint64 num);