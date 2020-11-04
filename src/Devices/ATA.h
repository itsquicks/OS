#pragma once
#include "VGA.h"
#include "../Typedefs.h"
#include "../IO.h"
#include "../Heap.h"

struct ATA
{
	uint16 dataPort;
	uint8 errorPort;
	uint8 sectorCountPort;
	uint8 lbaLowPort;
	uint8 lbaMidPort;
	uint8 lbaHighPort;
	uint8 devicePort;
	uint8 commandPort;
	uint8 controlPort;
	uint16 bytePerSector;
	bool master;
};

ATA* InitializeATA(uint16 portBase, bool master);
void ATA_Identify(ATA* ata);
void ATA_Read(ATA* ata, uint32 sector, uint8* data, uint16 count);
void ATA_Write(ATA* ata, uint32 sector, uint8* data, uint16 count);
void ATA_Flush(ATA* ata);