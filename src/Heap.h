#pragma once
#include "Typedefs.h"
#include "Memory.h"

struct MemorySegmentHeader
{
	uint64 MemoryLength;
	MemorySegmentHeader* NextSegment;
	MemorySegmentHeader* PreviousSegment;
	MemorySegmentHeader* NextFreeSegment;
	MemorySegmentHeader* PreviousFreeSegment;
	bool Free;
};

extern bool debug;

void InitializeHeap(uint64 heapAddress, uint64 heapLength);
void* calloc(uint64 size);
void* realloc(void* address, uint64 newSize);
void* malloc(uint64 size);
void free(void* address);