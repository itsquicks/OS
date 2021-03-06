#include "Filesystem.h"

ATA* hd;
MBR* mbr;
BPB* bpb;

uint32 fatStart;
uint32 dataStart;
uint32 rootStart;
uint32 dirStart;
char* path;

DirectoryEntry* entries;
uint8 entriesCount;
bool changed;

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
    hd = InitializeATA(0x1f0,false);
    //mbr = ReadMBR();
    //bpb = ReadBPB(mbr->primaryPartition[0].startLBA);
    bpb = ReadBPB(0);

    fatStart = bpb->reservedSectors;
    dataStart = fatStart + bpb->tableSize * bpb->fatCopies;
    rootStart = dataStart + bpb->sectorsPerCluster * (bpb->rootCluster - 2);
    dirStart = rootStart;

    path = (char*)calloc(72);
    stradd(path, "Root");

    entriesCount = 16 * bpb->sectorsPerCluster;
    entries = (DirectoryEntry*)calloc(entriesCount * sizeof(DirectoryEntry));
    ReadEntries();
}

void ReadEntries()
{
    //uint8 fatBuffer[513];
    uint32 dirSector = dirStart;
    //uint16 epc = 16 * bpb->sectorsPerCluster;

    //while (1)
    //{
        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            ATA_Read(hd, dirSector + i, (uint8*)&entries[16 * i], 512);
        }

        /*uint32 dirCluster = (dirSector - dataStart) / bpb->sectorsPerCluster + 2;
        uint32 fatSectorForCurrentCluster = dirCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = dirCluster % (512 / sizeof(uint32));
        dirCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;

        if (dirCluster == 0x00000000)
            return;

        if (dirCluster == 0x0fffffff)
            return;

        dirSector = dataStart + bpb->sectorsPerCluster * (dirCluster - 2);
        entriesCount += epc;
        entries = (DirectoryEntry*)realloc(entries, entriesCount * sizeof(DirectoryEntry));
    }*/
}

void WriteEntries()
{
    uint8 fatBuffer[513];
    uint32 dirSector = dirStart;

    uint16 j = 0;

    while (1)
    {
        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            ATA_Write(hd, dirSector + i, (uint8*)&entries[16 * i + 16 * j], 16 * sizeof(DirectoryEntry));
            ATA_Flush(hd);
        }

        uint32 dirCluster = (dirSector - dataStart) / bpb->sectorsPerCluster + 2;
        uint32 fatSectorForCurrentCluster = dirCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = dirCluster % (512 / sizeof(uint32));
        dirCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;

        if (dirCluster == 0x0fffffff)
            break;

        dirSector = dataStart + bpb->sectorsPerCluster * (dirCluster - 2);
        j++;
    }
}

char lfn[256];
char* GetLongFileName(uint8 index)
{
    DirectoryEntry* nextLFN = &entries[index];
    uint8 count = 1;

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
        index--;
        nextLFN = &entries[index];      
    }

    return lfn;
}

char* GetFileName(DirectoryEntry dir)
{
    uint8 index = 0;

    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (strcmp((char*)entries[i].name, (char*)dir.name, 11))
        {
            if (i == 0)
                index = i;
            else
                index = i - 1;

            break;
        }
    }

    if (dir.name[0] == '.')
    {
        if (dir.name[1] == 0x20)
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
        if (entries[index].attributes == 0x0f)
        {
            return GetLongFileName(index);
        }
        else
        {
            uint8 i;
            uint8 j;
            for (i = 0; i < 8; i++)
            {
                if (dir.name[i] == 0x20)
                    break;

                uint8 offset = 0;
                if ((dir.reserved & 0x08) == 0x08 && (dir.name[i] >= 'A' && dir.name[i] <= 'Z'))
                {
                    offset = 32;
                }

                *(lfn + i) = dir.name[i] + offset;
            }

            if (dir.ext[0] == 0x20)
            {
                *(lfn + i) = 0;
                return lfn;
            }

            *(lfn + i) = '.';
            i++;
            for (j = 0; j < 3; j++)
            {
                if (dir.ext[j] == 0x20)
                    break;

                uint8 offset = 0;
                if ((dir.reserved & 0x10) == 0x10 && (dir.ext[j] >= 'A' && dir.ext[j] <= 'Z'))
                {
                    offset = 32;
                }

                *(lfn + i + j) = dir.ext[j] + offset;
            }
            *(lfn + i + j) = 0;
            return lfn;
        }
    }
}

DirectoryEntry* DirectoryFromName(char* name)
{
    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (entries[i].name[0] == 0x00)
            return 0;

        if (entries[i].name[0] == 0xE5)
            continue;

        if ((entries[i].attributes & 0x0F) == 0x0F)
            continue;

        if ((entries[i].attributes & 0x08) == 0x08)
            continue;

        GetFileName(entries[i]);

        if (strcmp(name, lfn))
        {
            return &entries[i];
        }
    }

    return 0;
}

bool DOSNameFromString(char* str, uint8* name, uint8* ext, uint8& reserved)
{
    bool needLFN = false;
    uint8 len = strlen(str);

    for (uint8 i = 0; i < 8; i++)
        name[i] = 0x20;

    for (uint8 i = 0; i < 3; i++)
        ext[i] = 0x20;

    bool dot = false;
    uint8 doti = len - 1;
    while (doti != 0)
    {
        if (*(str + doti) == '.')
        {
            if (doti != 0)
                dot = true;
            else
                needLFN = true;
            break;
        }
        doti--;
    }

    if (dot)
    {
        if (doti < len - 4)
            needLFN = true;

        uint8 j = 0;
        for (uint8 i = doti + 1; i < len; i++)
        {
            char chr = *(str + i);

            if (j > 2)
                break;

            if (chr == 0x00)
            {
                break;
            }
            else if (chr >= 'a' && chr <= 'z')
            {
                ext[j] = chr - 32;
                j++;
            }
            else if (chr >= 'A' && chr <= 'Z')
            {
                ext[j] = chr;
                j++;
            }
            else if (chr >= '0' && chr <= '9')
            {
                ext[j] = chr;
                j++;
            }
            else
                needLFN = true;
        }
    }

    uint8 j = 0;
    if (doti == 0)
        doti = len;
    for (uint8 i = 0; i < doti; i++)
    {
        char chr = *(str + i);

        if (j > 7)
        {
            if (chr != 0x00)
                needLFN = true;
            break;
        }

        if (chr == 0x00)
        {
            break;
        }
        else if (chr >= 'a' && chr <= 'z')
        {
            name[j] = chr - 32;
            j++;
        }
        else if (chr >= 'A' && chr <= 'Z')
        {
            name[j] = chr;
            j++;
        }
        else if (chr >= '0' && chr <= '9')
        {
            name[j] = chr;
            j++;
        }
        else
            needLFN = true;
    }

    bool allCaps = true;
    bool noCaps = true;

    for (uint8 i = 0; i < doti; i++)
    {
        char chr = *(str + i);
        if (chr >= 'a' && chr <= 'z')
        {
            allCaps = false;
        }
        else if (chr >= 'A' && chr <= 'Z')
        {
            noCaps = false;
        }
    }

    if (allCaps)
    {
        reserved |= 0b00000000;
    }
    else if (noCaps)
    {
        reserved |= 0b00001000;
    }
    else
        needLFN = true;

    if (dot)
    {
        allCaps = true;
        noCaps = true;

        for (uint8 i = doti + 1; i < len; i++)
        {
            char chr = *(str + i);

            if (chr >= 'a' && chr <= 'z')
            {
                allCaps = false;
            }
            else if (chr >= 'A' && chr <= 'Z')
            {
                noCaps = false;
            }
        }

        if (allCaps)
        {
            reserved |= 0b00000000;
        }
        else if (noCaps)
        {
            reserved |= 0b00010000;
        }
        else
            needLFN = true;
    }

    if (needLFN)
    {
        reserved = 0;

        if (j == 8)
        {
            name[j - 2] = '~';
            name[j - 1] = '1';
        }
        else if (j == 7)
        {
            name[j - 1] = '~';
            name[j] = '1';
        }
        else
        {
            name[j] = '~';
            name[j + 1] = '1';
        }
    }

    return needLFN;
}

void Print_cTime(DirectoryEntry dir, uint8 color)
{
    bool pm = false;
    uint8 hours;
    uint8 minutes;

    hours = (dir.cTime & 0b1111100000000000) >> 11;
    if (hours > 12)
    {
        hours -= 12;
        pm = true;
    }

    minutes = (dir.cTime & 0b0000011111100000) >> 5;

    hours = IntAsHex(hours);
    minutes = IntAsHex(minutes);

    Print(HexToString(hours), color); Print(':', color); Print(HexToString(minutes), color); Print(' '); pm ? Print("PM", color) : Print("AM", color);
}

void Print_cDate(DirectoryEntry dir, uint8 color)
{
    uint8 day;
    uint8 month;
    uint16 year;

    day = dir.cDate & 0b0000000000011111l;
    month = (dir.cDate & 0b0000000111100000) >> 5;
    year = ((dir.cDate & 0b1111111000000000) >> 9) + 1980;

    day = IntAsHex(day);
    month = IntAsHex(month);
    year = IntAsHex(year);

    Print(HexToString(day), color);  Print('.', color);  Print(HexToString(month), color);  Print('.', color);  Print(HexToString(year), color);
}

uint32 GetFreeCluster()
{
    long size = bpb->tableSize;
    uint16 offset = 0;
    uint8 fatBuffer[513];

    while (size > 0)
    {
        ATA_Read(hd, fatStart + offset, fatBuffer, 512);

        for (uint8 i = 0; i < 512 / 4; i++)
        {
            uint32 value = ((uint32*)&fatBuffer)[i];

            if (value == 0x00000000)
                return i + offset * 128;
        }

        size--;
        offset++;
    }

    return 0;
}

void SetFAT(uint32 cluster, uint32 value)
{
    uint16 offset = cluster / 128;
    uint16 clusterRemainder = cluster % 128;
    uint8 fatBuffer[513];

    ATA_Read(hd, fatStart + offset, fatBuffer, 512);

    ((uint32*)&fatBuffer)[clusterRemainder] = value;

    ATA_Write(hd, fatStart + offset, fatBuffer, 512);
    ATA_Flush(hd);
    ATA_Write(hd, fatStart + offset + bpb->tableSize, fatBuffer, 512);
    ATA_Flush(hd);
}

void ChangeDirectory(DirectoryEntry dir)
{
    uint32 fileCluster = ((uint32)dir.firstClusterHigh << 16) | dir.firstClusterLow;
    uint32 fileSector = dataStart + bpb->sectorsPerCluster * (fileCluster - 2);
    
    if (fileSector < rootStart)
        fileSector = rootStart;

    dirStart = fileSector;

    ReadEntries();
}

void DeleteDirectory(DirectoryEntry dir)
{
    DirectoryEntry savedDir = dir;
    ChangeDirectory(dir);

    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (entries[i].name[0] == 0x00)
            break;

        if (entries[i].name[0] == 0xE5)
            continue;

        if (entries[i].name[0] == 0x2E)
            continue;

        if ((entries[i].attributes & 0x0F) == 0x0F)
            continue;

        if ((entries[i].attributes & 0x08) == 0x08)
            continue;

        if ((entries[i].attributes & 0x10) == 0x10)
        {
            DeleteDirectory(entries[i]);
        }
        else
        {
            DeleteFile(entries[i]);
        }
    }

    ChangeDirectory(entries[1]);
    DeleteFile(dir);
}

void DeleteFile(DirectoryEntry file, bool withContent)
{
    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (entries[i].name[0] == 0x00)
            break;

        if (entries[i].name[0] == 0xE5)
            continue;

        if ((entries[i].attributes & 0x0F) == 0x0F)
            continue;

        if ((entries[i].attributes & 0x08) == 0x08)
            continue;

        if (strcmp((char*)entries[i].name,(char*)file.name,11))
        {
            entries[i].name[0] = 0xE5;
            uint8 j = 1;

            while (entries[i - j].attributes == 0x0f)
            {
                entries[i - j].name[0] = 0xE5;
                j++;
            }

            WriteEntries();
            break;
        }
    }

    if (!withContent)
        return;

    uint32 firstFileCluster = ((uint32)file.firstClusterHigh << 16) | file.firstClusterLow;

    if (firstFileCluster == 0x00000000)
        return;

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
}

void NewFile(char* name, uint8 attributes)
{
    uint8 dosName[8];
    uint8 ext[3];
    uint8 reserved = 0;

    bool isLFN = DOSNameFromString(name, dosName, ext, reserved);

    DirectoryEntry file;

    for (uint8 i = 0; i < 8; i++)
        file.name[i] = dosName[i];

    for (uint8 i = 0; i < 3; i++)
        file.ext[i] = ext[i];

    file.attributes = attributes;
    file.reserved = reserved;
    file.cTimeTenth = 0;

    uint16 time = 0;
    uint16 date = 0;

    Read_RTC();
    
    date += HexAsInt(day);
    date += HexAsInt(month) << 5;
    date += ((century*100 + HexAsInt(year)) - 1980) << 9;

    time += HexAsInt(minute) << 5;
    time += HexAsInt(hour) << 11;

    file.cTime = time;
    file.cDate = date;
    file.wTime = time;
    file.wDate = date;  
    file.size = 0;

    if ((attributes & 0x10) == 0x10)
    {
        uint32 freeCluster = GetFreeCluster();
        SetFAT(freeCluster, 0x0fffffff);

        file.firstClusterHigh = (freeCluster & 0xff00) >> 16;
        file.firstClusterLow = freeCluster & 0x00ff;

        uint8* buffer = (uint8*)calloc(512);
        uint8* emptySpace = (uint8*)calloc(512);

        DirectoryEntry* dot = (DirectoryEntry*)buffer;
        DirectoryEntry* dotdot = (DirectoryEntry*)(buffer+32);

        for (uint8 i = 0; i < 8; i++)
            dot->name[i] = 0x20;

        for (uint8 i = 0; i < 3; i++)
            dot->ext[i] = 0x20;

        dot->name[0] = '.';
        dot->attributes = 0x10;
        dot->reserved = 0;
        dot->cTimeTenth = file.cTimeTenth;
        dot->cTime = file.cTime;
        dot->cDate = file.cDate;
        dot->wTime = file.wTime;
        dot->wDate = file.wDate;
        dot->size = 0;
        dot->firstClusterHigh = file.firstClusterHigh;
        dot->firstClusterLow = file.firstClusterLow;

        for (uint8 i = 0; i < 8; i++)
            dotdot->name[i] = 0x20;

        for (uint8 i = 0; i < 3; i++)
            dotdot->ext[i] = 0x20;

        dotdot->name[0] = '.';
        dotdot->name[1] = '.';
        dotdot->attributes = 0x10;
        dotdot->reserved = 0;
        dotdot->cTimeTenth = file.cTimeTenth;
        dotdot->cTime = file.cTime;
        dotdot->cDate = file.cDate;
        dotdot->wTime = file.wTime;
        dotdot->wDate = file.wDate;
        dotdot->size = 0;
        uint32 currentCluster = (dirStart - dataStart) / bpb->sectorsPerCluster + 2;
        dotdot->firstClusterHigh = (currentCluster & 0xff00) >> 16;
        dotdot->firstClusterLow = currentCluster & 0x00ff;
        
        uint32 fileSector = dataStart + bpb->sectorsPerCluster * (freeCluster - 2);

        ATA_Write(hd, fileSector, buffer, 512);
        ATA_Flush(hd);

        for (uint8 i = 1; i < bpb->sectorsPerCluster; i++)
        {
            ATA_Write(hd, fileSector + 1, emptySpace, 512);
            ATA_Flush(hd);
        }

        free(buffer);
        free(emptySpace);
    }
    else
    {
        file.firstClusterHigh = 0;
        file.firstClusterLow = 0;
    }
    
    uint8 lfnEntriesCount = 0;
    uint8 len = strlen(name);
    if (isLFN)
        lfnEntriesCount = len / 13 + (len % 13 == 0 ? 0 : 1);
    
    uint8 sum;
    uint8* ptr = (uint8*)&file.name;
    for (uint8 i = 11; i; i--)
        sum = ((sum & 1) << 7) + (sum >> 1) + *ptr++;

    uint8* lfnEntries = (uint8*)calloc(lfnEntriesCount * 32);

    uint8 l = 0;
    for (uint8 i = 0; i < lfnEntriesCount; i++)
    {   
        lfnEntries[32 * i] = i + 1;
        if (i == lfnEntriesCount - 1)
            lfnEntries[32 * i] |= 0x40;

        lfnEntries[32 * i + 0x0b] = 0x0f;
        lfnEntries[32 * i + 0x0d] = sum;

        for (uint8 j = 0; j < 10; j+=2)
        {
            if (l > len)
            {
                lfnEntries[32 * i + j + 1] = 0xff;
                lfnEntries[32 * i + j + 1 + 1] = 0xff;
            }
            else
            {
                lfnEntries[32 * i + j + 1] = name[l];
                lfnEntries[32 * i + j + 1 + 1] = 0x00;
                l++;
            }
        }

        for (uint8 j = 0; j < 12; j += 2)
        {
            if (l > len)
            {
                lfnEntries[32 * i + j + 0x0e] = 0xff;
                lfnEntries[32 * i + j + 0x0e + 1] = 0xff;
            }
            else
            {
                lfnEntries[32 * i + j + 0x0e] = name[l];
                lfnEntries[32 * i + j + 0x0e + 1] = 0x00;
                l++;
            }      
        }

        for (uint8 j = 0; j < 4; j += 2)
        {
            if (l > len)
            {
                lfnEntries[32 * i + j + 0x1c] = 0xff;
                lfnEntries[32 * i + j + 0x1c + 1] = 0xff;
            }
            else
            {
                lfnEntries[32 * i + j + 0x1c] = name[l];
                lfnEntries[32 * i + j + 0x1c + 1] = 0x00;
                l++;
            }
        }
    }

    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (entries[i].name[0] == 0xE5)
        {
            uint8 j;
            for (j = 1; j <= lfnEntriesCount; j++)
            {
                if (entries[i + j].name[0] != 0xE5 && entries[i + j].name[0] != 0x00)
                    break;
            }

            memcpy(&entries[i + j - 1], &file, 32);

            for (j = 0; j <lfnEntriesCount; j++)
                memcpy(&entries[i + j], &lfnEntries[32 * (lfnEntriesCount - 1 - j)], 32);
            
            WriteEntries();
            free(lfnEntries);
            return;
        }

        if (entries[i].name[0] == 0x00)
        {
            uint8 j;
            for (j = 1; j <= lfnEntriesCount; j++)
            {
                if (entries[i + j].name[0] != 0xE5 && entries[i + j].name[0] != 0x00)
                    break;
            }

            memcpy(&entries[i + j - 1], &file, 32);

            for (j = 0; j < lfnEntriesCount; j++)
                memcpy(&entries[i + j], &lfnEntries[32 * (lfnEntriesCount - 1 - j)], 32);

            WriteEntries();
            free(lfnEntries);
            return;
        }
    }
}

void RenameFile(DirectoryEntry file, char* newName)
{
    DirectoryEntry newFile = file;
    DeleteFile(file, false);

    uint8 dosName[8];
    uint8 ext[3];
    uint8 reserved = 0;

    bool isLFN = DOSNameFromString(newName, dosName, ext, reserved);

    for (uint8 i = 0; i < 8; i++)
        newFile.name[i] = dosName[i];

    for (uint8 i = 0; i < 3; i++)
        newFile.ext[i] = ext[i];

    uint8 lfnEntriesCount = 0;
    uint8 len = strlen(newName);
    if (isLFN)
        lfnEntriesCount = len / 13 + (len % 13 == 0 ? 0 : 1);

    uint8 sum;
    uint8* ptr = (uint8*)&newFile.name;
    for (uint8 i = 11; i; i--)
        sum = ((sum & 1) << 7) + (sum >> 1) + *ptr++;

    uint8* lfnEntries = (uint8*)calloc(lfnEntriesCount * 32);

    uint8 l = 0;
    for (uint8 i = 0; i < lfnEntriesCount; i++)
    {
        lfnEntries[32 * i] = i + 1;
        if (i == lfnEntriesCount - 1)
            lfnEntries[32 * i] |= 0x40;

        lfnEntries[32 * i + 0x0b] = 0x0f;
        lfnEntries[32 * i + 0x0d] = sum;

        for (uint8 j = 0; j < 10; j += 2)
        {
            if (l > len)
            {
                lfnEntries[32 * i + j + 1] = 0xff;
                lfnEntries[32 * i + j + 1 + 1] = 0xff;
            }
            else
            {
                lfnEntries[32 * i + j + 1] = newName[l];
                lfnEntries[32 * i + j + 1 + 1] = 0x00;
                l++;
            }
        }

        for (uint8 j = 0; j < 12; j += 2)
        {
            if (l > len)
            {
                lfnEntries[32 * i + j + 0x0e] = 0xff;
                lfnEntries[32 * i + j + 0x0e + 1] = 0xff;
            }
            else
            {
                lfnEntries[32 * i + j + 0x0e] = newName[l];
                lfnEntries[32 * i + j + 0x0e + 1] = 0x00;
                l++;
            }
        }

        for (uint8 j = 0; j < 4; j += 2)
        {
            if (l > len)
            {
                lfnEntries[32 * i + j + 0x1c] = 0xff;
                lfnEntries[32 * i + j + 0x1c + 1] = 0xff;
            }
            else
            {
                lfnEntries[32 * i + j + 0x1c] = newName[l];
                lfnEntries[32 * i + j + 0x1c + 1] = 0x00;
                l++;
            }
        }
    }

    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (entries[i].name[0] == 0xE5)
        {
            uint8 j;
            for (j = 1; j <= lfnEntriesCount; j++)
            {
                if (entries[i + j].name[0] != 0xE5 && entries[i + j].name[0] != 0x00)
                    break;
            }

            memcpy(&entries[i + j - 1], &newFile, 32);

            for (j = 0; j < lfnEntriesCount; j++)
                memcpy(&entries[i + j], &lfnEntries[32 * (lfnEntriesCount - 1 - j)], 32);

            WriteEntries();
            free(lfnEntries);
            return;
        }

        if (entries[i].name[0] == 0x00)
        {
            uint8 j;
            for (j = 1; j <= lfnEntriesCount; j++)
            {
                if (entries[i + j].name[0] != 0xE5 && entries[i + j].name[0] != 0x00)
                    break;
            }

            memcpy(&entries[i + j - 1], &newFile, 32);

            for (j = 0; j < lfnEntriesCount; j++)
                memcpy(&entries[i + j], &lfnEntries[32 * (lfnEntriesCount - 1 - j)], 32);

            WriteEntries();
            free(lfnEntries);
            return;
        }
    }
}

void LoadFile(DirectoryEntry file, uint64 address)
{
    uint32 firstFileCluster = ((uint32)file.firstClusterHigh << 16) | file.firstClusterLow;

    uint32 size = file.size;
    uint8* ptr = (uint8*)address;

    uint32 nextFileCluster = firstFileCluster;
    uint8 fatBuffer[513];

    while (1)
    {
        uint32 fileSector = dataStart + bpb->sectorsPerCluster * (nextFileCluster - 2);

        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            uint32 readSize = size > 512 ? 512 : size;
            ATA_Read(hd, fileSector + i, ptr, readSize);
            ptr += readSize;
            size -= readSize;

            if (size == 0)
                break;
        }

        uint32 fatSectorForCurrentCluster = nextFileCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32));
        nextFileCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;

        if (nextFileCluster == 0x0fffffff)
            break;
    }
}

void SaveFile(DirectoryEntry file, uint64 address, uint32 newSize)
{
    uint32 firstFileCluster = ((uint32)file.firstClusterHigh << 16) | file.firstClusterLow;

    bool empty = false;
    if (firstFileCluster == 0)
    {
        firstFileCluster = GetFreeCluster();
        SetFAT(firstFileCluster, 0x0fffffff);
        empty = true;
    }

    uint32 _newSize = newSize;
    uint8* ptr = (uint8*)address;
    uint32 nextFileCluster = firstFileCluster;
    uint8 fatBuffer[513];

    while (1)
    {
        uint32 fileSector = dataStart + bpb->sectorsPerCluster * (nextFileCluster - 2);

        for (uint8 i = 0; i < bpb->sectorsPerCluster; i++)
        {
            uint32 writeSize = newSize > 512 ? 512 : newSize;
            ATA_Write(hd, fileSector + i, ptr, writeSize);
            ATA_Flush(hd);
            ptr += writeSize;
            newSize -= writeSize;

            if (newSize == 0)
                break;
        }

        uint32 fatSectorForCurrentCluster = nextFileCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32));
        nextFileCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;

        if (nextFileCluster == 0x0fffffff)
            break;
    }

    for (uint8 i = 0; i < entriesCount; i++)
    {
        if (entries[i].name[0] == 0x00)
            return;

        if (entries[i].name[0] == 0xE5)
            continue;

        if ((entries[i].attributes & 0x0F) == 0x0F)
            continue;

        if ((entries[i].attributes & 0x08) == 0x08)
            continue;

        if (strcmp((char*)file.name,(char*)entries[i].name,11))
        {
            entries[i].size = _newSize;

            if (empty)
            {
                entries[i].firstClusterHigh = (firstFileCluster & 0xffff0000) >> 16;
                entries[i].firstClusterLow = (uint16)firstFileCluster;
            }

            WriteEntries();
            return;
        }
    } 
}

void PrintDirectoryContent()
{
    for (uint8 i = 0; i < entriesCount; i++)
    {
        uint8 color = whiteF;

        if (entries[i].name[0] == 0x00)
            return;

        if (entries[i].name[0] == 0xE5)
            continue;

        if ((entries[i].attributes & 0x0F) == 0x0F)
            continue;

        if ((entries[i].attributes & 0x08) == 0x08)
            continue;

        if ((entries[i].attributes & 0x02) == 0x02)
            color = darkgrayF;

        if ((entries[i].attributes & 0x10) == 0x10)
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

        GetFileName(entries[i]);

        Print(lfn, color);
        SetCursorPosition(40, cursorPos / 80);
        PrintBack(IntToString(entries[i].size), color);  Print(" Bytes       ", color);
        Print_cTime(entries[i], color);  Print("       ", color);
        Print_cDate(entries[i], color); Print("\r\n");
    }
}

void PrintFileContent(DirectoryEntry file)
{
    uint32 firstFileCluster = ((uint32)file.firstClusterHigh << 16) | file.firstClusterLow;  

    uint32 size = file.size;
    uint32 nextFileCluster = firstFileCluster;

    uint8 buffer[513];
    uint8 fatBuffer[513];

    while (1)
    {
        uint32 fileSector = dataStart + bpb->sectorsPerCluster * (nextFileCluster - 2);

        for (uint8 i = 0; i< bpb->sectorsPerCluster; i++)
        {
            uint32 readSize = size > 512 ? 512 : size;
            ATA_Read(hd, fileSector + i, (uint8*)&buffer, readSize);

            size -= readSize;
            buffer[readSize] = 0;
            Print((char*)buffer);

            if (size == 0)
                break;
        }

        uint32 fatSectorForCurrentCluster = nextFileCluster / (512 / sizeof(uint32));
        ATA_Read(hd, fatStart + fatSectorForCurrentCluster, fatBuffer, 512);
        uint32 fatOffsetInSectorForCurrentCluster = nextFileCluster % (512 / sizeof(uint32));
        nextFileCluster = ((uint32*)&fatBuffer)[fatOffsetInSectorForCurrentCluster] & 0x0fffffff;

        if (nextFileCluster == 0x0fffffff)
            break;
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