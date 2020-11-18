#include "Heap.h"

MemorySegmentHeader* FirstFreeMemorySegment;

bool debug = false;

void InitializeHeap(uint64 heapAddress, uint64 heapLength)
{
	FirstFreeMemorySegment = (MemorySegmentHeader*)heapAddress;
	FirstFreeMemorySegment->MemoryLength = heapLength - sizeof(MemorySegmentHeader);
	FirstFreeMemorySegment->NextSegment = 0;
	FirstFreeMemorySegment->PreviousSegment = 0;
	FirstFreeMemorySegment->NextFreeSegment = 0;
	FirstFreeMemorySegment->PreviousFreeSegment = 0;
	FirstFreeMemorySegment->Free = true;
}

void* calloc(uint64 size)
{
	void* mallocVal = malloc(size);
	memset(mallocVal, 0, size);
	return mallocVal;
}

void* realloc(void* ptr1, uint64 length) {

	void* ptr2 = calloc(length);
	uint64 new_ptr_length = 0;
	MemorySegmentHeader* memSeg = (((MemorySegmentHeader*)ptr1) - 1);

	if (!ptr2) return 0;

	if (memSeg->MemoryLength > length)
		new_ptr_length = length;
	else
		new_ptr_length = memSeg->MemoryLength;

	memcpy(ptr2, ptr1, new_ptr_length);
	free(ptr1);

	return ptr2;
}
void* malloc(uint64 size)
{
	uint64 remainder = size % 8;
	size -= remainder;
	if (remainder != 0) size += 8;

	MemorySegmentHeader* currentMemorySegment = FirstFreeMemorySegment;

	while (true)
	{
		Print(HexToString((uint64)currentMemorySegment),0x1d);
		Print("\r\n");
		if (currentMemorySegment->MemoryLength >= size)
		{
			if (currentMemorySegment->MemoryLength > size + sizeof(MemorySegmentHeader))
			{
				MemorySegmentHeader* newSegmentHeader = (MemorySegmentHeader*)((uint64)currentMemorySegment + sizeof(MemorySegmentHeader) + size);
				newSegmentHeader->Free = true;
				newSegmentHeader->MemoryLength = ((uint64)currentMemorySegment->MemoryLength) - (sizeof(MemorySegmentHeader) + size);
				newSegmentHeader->NextFreeSegment = currentMemorySegment->NextFreeSegment;
				newSegmentHeader->NextSegment = currentMemorySegment->NextSegment;
				newSegmentHeader->PreviousSegment = currentMemorySegment;
				newSegmentHeader->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;

				currentMemorySegment->NextFreeSegment = newSegmentHeader;
				currentMemorySegment->NextSegment = newSegmentHeader;
				currentMemorySegment->MemoryLength = size;
			}

			if (currentMemorySegment == FirstFreeMemorySegment)
				FirstFreeMemorySegment = currentMemorySegment->NextFreeSegment;

			currentMemorySegment->Free = false;

			if (currentMemorySegment->PreviousFreeSegment != 0) currentMemorySegment->PreviousFreeSegment->NextFreeSegment = currentMemorySegment->NextFreeSegment;
			if (currentMemorySegment->NextFreeSegment != 0) currentMemorySegment->NextFreeSegment->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;
			if (currentMemorySegment->PreviousSegment != 0) currentMemorySegment->PreviousSegment->NextFreeSegment = currentMemorySegment->NextFreeSegment;
			if (currentMemorySegment->NextSegment != 0) currentMemorySegment->NextSegment->PreviousFreeSegment = currentMemorySegment->PreviousFreeSegment;

			if (debug)
			{
				Print(IntToString(allocatedMemSize), 0x1f);
				Print(" += ", 0x1a);
				Print(IntToString(size), 0x1f);
				Print(" -> ", 0x1f);
				Print(HexToString((uint32)(uint64)(currentMemorySegment + 1)), 0x1f);
				Print("   ",0x1f);

				Print(IntToString(currentMemorySegment->MemoryLength),0x10 | yellowF);
				Print("\r\n");
			}

			allocatedMemSize += currentMemorySegment->MemoryLength;
			return currentMemorySegment + 1;
		}

		if (currentMemorySegment->NextFreeSegment == 0)
			return 0;

		currentMemorySegment = currentMemorySegment->NextFreeSegment;
	}

	return 0;
}

void CombineFreeSegments(MemorySegmentHeader* a, MemorySegmentHeader* b)
{
	if (a == 0) return;
	if (b == 0) return;
	if (a == b) return;

	if (a < b)
	{
		a->MemoryLength += b->MemoryLength + sizeof(MemorySegmentHeader);
		a->NextSegment = b->NextSegment;
		a->NextFreeSegment = b->NextFreeSegment;
		b->NextSegment->PreviousSegment = a;
		b->NextSegment->PreviousFreeSegment = a;
		b->NextFreeSegment->PreviousFreeSegment = a;
	}
	else
	{
		b->MemoryLength += a->MemoryLength + sizeof(MemorySegmentHeader);
		b->NextSegment = a->NextSegment;
		b->NextFreeSegment = a->NextFreeSegment;
		a->NextSegment->PreviousSegment = b;
		a->NextSegment->PreviousFreeSegment = b;
		a->NextFreeSegment->PreviousFreeSegment = b;
	}
}

void free(void* address)
{
	MemorySegmentHeader* currentMemorySegment = ((MemorySegmentHeader*)address) - 1;

	if (debug)
	{
		Print(IntToString(allocatedMemSize),0x1f);
		Print(" -= ", 0x1c);
		Print(IntToString(currentMemorySegment->MemoryLength), 0x1f);
		Print(" <- " ,0x1f);
		Print(HexToString((uint32)(uint64)address), 0x1f);
		Print("   ",0x1f);

		Print(IntToString(currentMemorySegment->MemoryLength), 0x10 | yellowF);
		Print("\r\n");
	}

	allocatedMemSize -= currentMemorySegment->MemoryLength;
	currentMemorySegment->Free = true;

	if (currentMemorySegment < FirstFreeMemorySegment) FirstFreeMemorySegment = currentMemorySegment;

	if (currentMemorySegment->NextFreeSegment != 0)
	{
		if (currentMemorySegment->NextFreeSegment->PreviousFreeSegment < currentMemorySegment)
			currentMemorySegment->NextFreeSegment->PreviousFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->PreviousFreeSegment != 0)
	{
		if (currentMemorySegment->PreviousFreeSegment->NextFreeSegment > currentMemorySegment)
			currentMemorySegment->PreviousFreeSegment->NextFreeSegment = currentMemorySegment;
	}
	if (currentMemorySegment->NextSegment != 0)
	{
		currentMemorySegment->NextSegment->PreviousSegment = currentMemorySegment;
		currentMemorySegment->NextSegment->PreviousFreeSegment = currentMemorySegment;
		if (currentMemorySegment->NextSegment->Free) CombineFreeSegments(currentMemorySegment, currentMemorySegment->NextSegment);
	}
	if (currentMemorySegment->PreviousSegment != 0)
	{
		currentMemorySegment->PreviousSegment->NextSegment = currentMemorySegment;
		currentMemorySegment->PreviousSegment->NextFreeSegment = currentMemorySegment;
		if (currentMemorySegment->PreviousSegment->Free) CombineFreeSegments(currentMemorySegment, currentMemorySegment->PreviousSegment);
	}
}