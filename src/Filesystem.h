#pragma once
#include "Typedefs.h"
#include "Heap.h"
#include "Utils.h"
#include "Devices/ATA.h"
#include "Devices/VGA.h"
#include "Devices/CMOS.h"

struct PartitionTableEntry
{
	uint8 status;
	uint8 startHead;
	uint8 startSector;
	uint8 startCylinder;
	uint8 type;
	uint8 endHead;
	uint8 endSector;
	uint8 endCylinder;
	uint32 startLBA;
	uint32 length;

}__attribute__((packed));

struct MBR
{
	uint8 bootloader[440];
	uint32 signature;
	uint16 unused;
	PartitionTableEntry primaryPartition[4];
	uint16 magicNumber;

}__attribute__((packed));

struct BPB
{
	uint8 jump[3];
	uint8 softName[8];
	uint16 bytesPerSector;
	uint8 sectorsPerCluster;
	uint16 reservedSectors;
	uint8 fatCopies;
	uint16 rootDirEntries;
	uint16 totalSectors;
	uint8 mediaType;
	uint16 fatSectorCount;
	uint16 sectorsPerTrack;
	uint16 headCount;
	uint32 hiddenSectors;
	uint32 totalSectorCount;

	uint32 tableSize;
	uint16 extFlags;
	uint16 fatVersion;
	uint32 rootCluster;
	uint16 fatInfo;
	uint16 backupSector;
	uint8 reserved0[12];
	uint8 driveNumber;
	uint8 reserved;
	uint8 bootSignature;
	uint32 volumeID;
	uint8 volumeLabel[11];
	uint8 fatTypeLabel[8];

}__attribute__((packed));

struct DirectoryEntry
{
	uint8 name[8];
	uint8 ext[3];
	uint8 attributes;
	uint8 reserved;
	uint8 cTimeTenth;
	uint16 cTime;
	uint16 cDate;
	uint16 aTime;
	uint16 firstClusterHigh;
	uint16 wTime;
	uint16 wDate;
	uint16 firstClusterLow;
	uint32 size;

}__attribute__((packed));

extern ATA* hd;
extern MBR* mbr;
extern BPB* bpb;

extern uint32 fatStart;
extern uint32 dataStart;
extern uint32 rootStart;
extern uint32 dirStart;
extern char* path;

extern DirectoryEntry* entries;
extern uint8 entriesCount;

MBR* ReadMBR();
BPB* ReadBPB(uint32 partitionOffset);

void InitializeFilesystem();
void ReadEntries();
void WriteEntries();

char* GetLongFileName(uint8 index);
char* GetFileName(DirectoryEntry dir);
DirectoryEntry* DirectoryFromName(char* name);
bool DOSNameFromString(char* str, uint8* name, uint8* ext, uint8& reserved);
void Print_cTime(DirectoryEntry dir, uint8 color = whiteF);
void Print_cDate(DirectoryEntry dir, uint8 color = whiteF);

uint32 GetFreeCluster();
void SetFAT(uint32 cluster, uint32 value);

void ChangeDirectory(DirectoryEntry dir);
void DeleteDirectory(DirectoryEntry dir);
void DeleteFile(DirectoryEntry file, bool withContent = true);
void NewFile(char* name, uint8 attributes);
void RenameFile(DirectoryEntry file, char* newName);
void LoadFile(DirectoryEntry file, uint64 address);
void SaveFile(DirectoryEntry file, uint64 address, uint32 newSize);

void PrintDirectoryContent();
void PrintFileContent(DirectoryEntry file);
void PrintMBR(MBR* mbr);
void PrintBPB(BPB* bpb);