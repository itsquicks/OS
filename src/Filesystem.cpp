#include "Filesystem.h"

ATA* hd;
MBR* mbr;
BPB* bpb;

uint32 fatStart;
uint32 dataStart;
uint32 rootStart;
uint32 dirStart;

char* path;

DirectoryEntry** dirEntries;

MBR* ReadMBR()
{
    MBR* mbr = (MBR*)calloc(sizeof(MBR));
    ATA_Read(hd, 0, (uint8*)mbr, sizeof(MBR));

    return mbr;
}

BPB* ReadBPB(uint32 partitionOffset)
{
    BPB* bpb = (BPB*)calloc(sizeof(BPB));
    ATA_Read(hd, partitionOffset, (uint8*)bpb, sizeof(BPB));

    return bpb;
}

void InitializeFilesystem()
{
    hd = InitializeATA(0x1f0, false);
    //mbr = ReadMBR();
    //bpb = ReadBPB(mbr->primaryPartition[0].startLBA);
    bpb = ReadBPB(0);

    fatStart = bpb->reservedSectors;
    dataStart = fatStart + bpb->tableSize * bpb->fatCopies;
    rootStart = dataStart + bpb->sectorsPerCluster * (bpb->rootCluster - 2);
    dirStart = rootStart;

    path = (char*)calloc(65);
    stradd(path, "Root");
}

char* lfn = (char*)calloc(256);
char* GetLongFileName(DirectoryEntry* dir)
{
    DirectoryEntry* nextLFN = (DirectoryEntry*)(dir - 1);
    uint8 count = 01;

    while (1)
    {
        if (nextLFN->attributes != 0x0F)
            break;

        *(lfn + 13 * (count - 1) + 0) = *((uint8*)nextLFN + 1);
        *(lfn + 13 * (count - 1) + 1) = *((uint8*)nextLFN + 3);
        *(lfn + 13 * (count - 1) + 2) = *((uint8*)nextLFN + 5);
        *(lfn + 13 * (count - 1) + 3) = *((uint8*)nextLFN + 7);
        *(lfn + 13 * (count - 1) + 4) = *((uint8*)nextLFN + 9);
        *(lfn + 13 * (count - 1) + 5) = *((uint8*)nextLFN + 14);
        *(lfn + 13 * (count - 1) + 6) = *((uint8*)nextLFN + 16);
        *(lfn + 13 * (count - 1) + 7) = *((uint8*)nextLFN + 18);
        *(lfn + 13 * (count - 1) + 8) = *((uint8*)nextLFN + 20);
        *(lfn + 13 * (count - 1) + 9) = *((uint8*)nextLFN + 22);
        *(lfn + 13 * (count - 1) + 10) = *((uint8*)nextLFN + 24);
        *(lfn + 13 * (count - 1) + 11) = *((uint8*)nextLFN + 28);
        *(lfn + 13 * (count - 1) + 12) = *((uint8*)nextLFN + 30);

        if (nextLFN->name[0] == 0x40 + count)
            break;

        count++;
        nextLFN -= 1;
    }

    return lfn;
}

char* GetFileName(DirectoryEntry* dir)
{
    if (dir->name[0] == '.')
    {
        if (dir->name[1] == 0x20)
        {
            *(lfn) = '.';
            *(lfn + 1) = 0;
            return lfn;
        }
        else
        {
            *(lfn) = '.';
            *(lfn + 1) = '.';
            *(lfn + 2) = 0;
            return lfn;
        }
    }
    else
    {
        if ((dir - 1)->attributes == 0x0f)
        {
            return GetLongFileName(dir);
        }
        else
        {
            uint8 i;
            uint8 j;
            for (i = 0; i < 8; i++)
            {
                if (dir->name[i] == 0x20)
                    break;

                uint8 offset = 0;
                if ((dir->reserved & 0x08) == 0x08 && (dir->name[i] >= 'A' && dir->name[i] <= 'Z'))
                {
                    offset = 32;
                }

                *(lfn + i) = dir->name[i] + offset;
            }

            if ((dir->attributes & 0x10) == 0x10)
            {
                *(lfn + i) = 0;
                return lfn;
            }

            *(lfn + i) = '.';
            i++;
            for (j = 0; j < 3; j++)
            {
                if (dir->ext[j] == 0x20)
                    break;

                uint8 offset = 0;
                if ((dir->reserved & 0x10) == 0x10 && (dir->ext[j] >= 'A' && dir->ext[j] <= 'Z'))
                {
                    offset = 32;
                }

                *(lfn + i + j) = dir->ext[j] + offset;
            }
            *(lfn + i + j) = 0;
            return lfn;
        }
    }
}

DirectoryEntry* DirectoryFromName(char* name)
{
    bool end = false;

    DirectoryEntry dirent[16 * bpb->sectorsPerCluster];
    uint8 fatBuffer[513];
    uint32 dirSector = dirStart;

    while (1)
    {
        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            ATA_Read(hd, dirSector + i, (uint8*)(&dirent[16 * i]), 16 * sizeof(DirectoryEntry));
        }

        for (uint8 i = 0; i < 16 * bpb->sectorsPerCluster; i++)
        {
            if (dirent[i].name[0] == 0x00)
            {
                end = true;
                break;
            }

            if (dirent[i].name[0] == 0xE5)
                continue;

            if ((dirent[i].attributes & 0x0F) == 0x0F)
                continue;

            if ((dirent[i].attributes & 0x08) == 0x08)
                continue;

            GetFileName((DirectoryEntry*)&dirent[i]);
            if (strcmp(name, lfn))
            {
                uint64 address = (uint64)&dirent[i];
                return (DirectoryEntry*)address;
            }
        }

        if (end)
            break;
        else
        {
            uint32 dirCluster = (dirSector - dataStart) / bpb->sectorsPerCluster + 2;
            uint32 fatSectorForCurrentCluster = dirCluster / (512 / sizeof(uint32));
            ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
            uint32 fatOffsetInSectorForCurrentCluster = dirCluster % (512 / sizeof(uint32));
            dirCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;
            dirSector = dataStart + bpb->sectorsPerCluster * (dirCluster - 2);
        }
    }

    return 0;
}

uint32 GetFreeCluster()
{
    uint32 ret;


    return ret;
}

DirectoryEntry** NewFile(char* name, uint8 attributes)
{

}

void DeleteFile(DirectoryEntry* file)
{
    /*uint32 firstFileCluster = ((uint32)file->firstClusterHigh << 16) | file->firstClusterLow;
    DirectoryEntry dir = *file;
    uint32 nextFileCluster = firstFileCluster;
    uint8 fatBuffer[513];

    while (1)
    {
        uint32 fatSectorForCurrentCluster = nextFileCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32));
        nextFileCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;

        if (nextFileCluster == 0x00000000)
            break;

        ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] = 0x00000000;

        ATA_Write(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        ATA_Flush(hd);
        ATA_Write(hd, fatStart + fatSectorForCurrentCluster + bpb->tableSize, fatBuffer, 512);
        ATA_Flush(hd);

        if (nextFileCluster == 0x0fffffff)
            break;
    }

    bool end = false;

    DirectoryEntry dirent[16 * bpb->sectorsPerCluster];
    uint32 dirSector = dirStart;
    uint32 fatSectorForCurrentCluster;

    while (1)
    {
        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            ATA_Read(hd, dirSector + i, (uint8*)(&dirent[16 * i]), 16 * sizeof(DirectoryEntry));
        }

        for (uint8 i = 0; i < 16 * bpb->sectorsPerCluster; i++)
        {
            if (dirent[i].name[0] == 0x00)
            {
                end = true;
                break;
            }

            if (dirent[i].name[0] == 0xE5)
                continue;

            if ((dirent[i].attributes & 0x0F) == 0x0F)
                continue;

            if ((dirent[i].attributes & 0x08) == 0x08)
                continue;

            Print((char*)dir.name);
            Print("\r\n");
            Print((char*)dirent[i].name);
            Print("\r\n");
            if (dirent[i].name == dir.name)
            {
                Print("A");
                end = true;
                dirent[i].name[0] = 0xE5;
                uint8 j = 1;
                while (dirent[i - j].attributes == 0x0f)
                {
                    dirent[i - j].name[0] = 0xE5; 
                    j++;
                }

                for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
                {
                    ATA_Write(hd, dirSector + i, (uint8*)(&dirent[16 * i]), 512);
                    ATA_Flush(hd);
                }

                break;
            }
        }

        if (end)
            break;
        else
        {
            uint32 dirCluster = (dirSector - dataStart) / bpb->sectorsPerCluster + 2;
            uint32 fatSectorForCurrentCluster = dirCluster / (512 / sizeof(uint32));
            ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
            uint32 fatOffsetInSectorForCurrentCluster = dirCluster % (512 / sizeof(uint32));
            dirCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;
            dirSector = dataStart + bpb->sectorsPerCluster * (dirCluster - 2);
        }
    }*/
}

void ChangeDirectory(DirectoryEntry* dir)
{
    uint32 fileCluster = ((uint32)dir->firstClusterHigh << 16) | dir->firstClusterLow;
    uint32 fileSector = dataStart + bpb->sectorsPerCluster * (fileCluster - 2);

    if (fileSector < rootStart)
        fileSector = rootStart;

    dirStart = fileSector;
}

void PrintDirectoryContent()
{
    bool end = false;

    DirectoryEntry dirent[16 * bpb->sectorsPerCluster];
    uint8 fatBuffer[513];
    uint32 dirSector = dirStart;
    
    while (1)
    {
        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            ATA_Read(hd, dirSector + i, (uint8*)(&dirent[16 * i]), 16 * sizeof(DirectoryEntry));
        }

        for (uint8 i = 0; i < 16 * bpb->sectorsPerCluster; i++)
        {
            uint8 color = whiteF;

            if (dirent[i].name[0] == 0x00)
            {
                end = true;
                break;
            }

            if (dirent[i].name[0] == 0xE5)
                continue;

            if ((dirent[i].attributes & 0x0F) == 0x0F)
                continue;

            if ((dirent[i].attributes & 0x08) == 0x08)
                continue;

            if ((dirent[i].attributes & 0x02) == 0x02)
                color = darkgrayF;

            if ((dirent[i].attributes & 0x10) == 0x10)
            {
                Print(' ', color);
                Print(240, color);
                Print(' ', color);
            }
            else
            {
                Print(' ', color);
                Print(0xfe, color);
                Print(' ', color);
            }

            GetFileName((DirectoryEntry*)&dirent[i]);
            Print(lfn, color);
            SetCursorPosition(40, cursorPos / 80); PrintBack(IntToString(dirent[i].size), color);  Print(" Bytes", color);
            Print("\r\n");
        }

        if (end)
            break;
        else
        {
            uint32 dirCluster = (dirSector - dataStart) / bpb->sectorsPerCluster + 2;
            uint32 fatSectorForCurrentCluster =dirCluster / (512 / sizeof(uint32));
            ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
            uint32 fatOffsetInSectorForCurrentCluster = dirCluster % (512 / sizeof(uint32));
            dirCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;
            dirSector = dataStart + bpb->sectorsPerCluster * (dirCluster - 2);
        }
    }
}

void PrintFileContent(DirectoryEntry* file)
{
    uint32 firstFileCluster = ((uint32)file->firstClusterHigh << 16) | file->firstClusterLow;  

    long size = (long)file->size;
    uint32 nextFileCluster = firstFileCluster;
    uint8 buffer[513];
    uint8 fatBuffer[513];

    while (size > 0)
    {
        uint32 fileSector = dataStart + bpb->sectorsPerCluster * (nextFileCluster - 2);
        int sectorOffset = 0;
        for (; size > 0; size -= 512)
        {
            ATA_Read(hd, fileSector + sectorOffset, (uint8*)&buffer, 512);
            buffer[size > 512 ? 512 : size] = 0;
            Print((char*)buffer);
            if (++sectorOffset > bpb->sectorsPerCluster)
                break;
        }

        uint32 fatSectorForCurrentCluster = nextFileCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32));
        nextFileCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;
    }
}

void PrintMBR(MBR* mbr)
{
    Print("Signature: ", cyanF); Print(HexToString(mbr->signature));  Print("\r\n");
    Print("Status : ", cyanF); Print(HexToString(mbr->primaryPartition[0].status));  Print("\r\n");
    Print("Partition Type : ", cyanF); Print(HexToString(mbr->primaryPartition[0].type));  Print("\r\n");
    Print("LBA Start : ", cyanF); Print(IntToString(mbr->primaryPartition[0].startLBA));  Print("\r\n");
    Print("Length : ", cyanF); Print(IntToString(mbr->primaryPartition[0].length));  Print("\r\n");
    Print("Status : ", cyanF); Print(HexToString(mbr->primaryPartition[1].status));  Print("\r\n");
    Print("Partition Type : ", cyanF); Print(HexToString(mbr->primaryPartition[1].type));  Print("\r\n");
    Print("LBA Start : ", cyanF); Print(IntToString(mbr->primaryPartition[1].startLBA));  Print("\r\n");
    Print("Length : ", cyanF); Print(IntToString(mbr->primaryPartition[1].length));  Print("\r\n");
    Print("Status : ", cyanF); Print(HexToString(mbr->primaryPartition[2].status));  Print("\r\n");
    Print("Partition Type : ", cyanF); Print(HexToString(mbr->primaryPartition[2].type));  Print("\r\n");
    Print("LBA Start : ", cyanF); Print(IntToString(mbr->primaryPartition[2].startLBA));  Print("\r\n");
    Print("Length : ", cyanF); Print(IntToString(mbr->primaryPartition[2].length));  Print("\r\n");
    Print("Status : ", cyanF); Print(HexToString(mbr->primaryPartition[3].status));  Print("\r\n");
    Print("Partition Type : ", cyanF); Print(HexToString(mbr->primaryPartition[3].type));  Print("\r\n");
    Print("LBA Start : ", cyanF); Print(IntToString(mbr->primaryPartition[3].startLBA));  Print("\r\n");
    Print("Length : ", cyanF); Print(IntToString(mbr->primaryPartition[3].length));  Print("\r\n");
}

void PrintBPB(BPB* bpb)
{
    Print("Volume label : ", cyanF);
    for (uint8 j = 0; j < 11; j++)
        Print(bpb->volumeLabel[j]);
    Print("\r\n");

    Print("Bytes per sector : ", cyanF); Print(IntToString(bpb->bytesPerSector));  Print("\r\n");
    Print("Sectors per cluster : ", cyanF); Print(IntToString(bpb->sectorsPerCluster));  Print("\r\n");
    Print("Table size : ", cyanF); Print(IntToString(bpb->tableSize));  Print("\r\n");
    Print("FAT copies : ", cyanF); Print(IntToString(bpb->fatCopies));  Print("\r\n");
    Print("Total sector count : ", cyanF); Print(IntToString(bpb->totalSectorCount));  Print("\r\n");
}