#pragma once
#include "Typedefs.h"
#include "Heap.h"

uint16 strlen(const char* str);
bool strcmp(const char* str1, const char* str2);
void stradd(char* str1, const char* str2);
void stradd(char* str, char chr);

uint8 wordcount(const char* msg);
char** splitwords(const char* msg, uint8 words);
void shiftup(char* msg, uint8 index, char chr);
void shiftdown(char* msg, uint8 index);

uint64 StringToHex(const char* str);
uint64 StringToInt(const char* str);

const char* HexToString(uint8 value);
const char* HexToString(uint16 value);
const char* HexToString(uint32 value);
const char* HexToString(uint64 value);
const char* HexToString(char value);
const char* HexToString(short value);
const char* HexToString(int value);
const char* HexToString(long long value);

extern const char* IntToString(uint8 value);
extern const char* IntToString(uint16 value);
extern const char* IntToString(uint32 value);
extern const char* IntToString(uint64 value);
const char* IntToString(char value);
const char* IntToString(short value);
const char* IntToString(int value);
const char* IntToString(long long value);

const char* FloatToString(float value, uint8 decimals);