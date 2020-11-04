#include "Utils.h"

uint16 strlen(const char* str)
{
	char* ptr = (char*)str;
	uint16 ret = 0;

	while (*ptr != 0)
	{
		ptr++;
		ret++;
	}

	return ret;
}

bool strcmp(const char* str1, const char* str2)
{
	const char* _str1 = str1;
	const char* _str2 = str2;

	while (*_str1 != 0)
	{
		if (*_str1 != *_str2)
			return false;

		_str1++;
		_str2++;
	}

	if (*_str2 != 0)
		return false;

	return true;
}

void stradd(char* str1, const char* str2)
{
	uint8 len = strlen(str1);
	uint8 i = 0;
	while (*(str2 + i) != 0)
	{
		*(str1 + len + i) = *(str2 + i);
		i++;
	}
	*(str1 + len + i) = 0;
}

void stradd(char* str, char chr)
{
	uint8 len = strlen(str);
	*(str + len) = chr;
	*(str + len + 1) = 0;
}

uint8 wordcount(const char* msg)
{
	char* ptr = (char*)msg;
	uint8 count = 0;
	bool quotes = false;

	if (*ptr != 0x20 && *ptr != 0)
		count++;

	while (*ptr != 0)
	{

		if (*(ptr + 1) != 0)
		{
			if (!quotes)
			{
				if (*ptr == 0x20 && *(ptr + 1) != 0x20)
				{
					count++;
				}
			}

			if (*(ptr + 1) == '"')
				quotes = !quotes;
		}
		else
		{
			return count;
		}
		ptr++;
	}

	return count;
}

char** splitwords(const char* msg, uint8 words)
{
	char** argv = (char**)calloc(words * sizeof(char*));
	for (uint8 i = 0; i < words; i++)
	{
		argv[i] = (char*)calloc(33 * sizeof(char));
	}

	char* ptr = (char*)msg;
	uint8 i = 0;
	uint8 j = 0;
	bool isEmpty = false;
	bool quotes = false;

	if (*ptr == 0x20)
		isEmpty = true;

	while (*ptr != 0)
	{
		if (!quotes)
		{
			if (*ptr == 0x20)
			{
				if (!isEmpty)
				{
					*(argv[i] + j) = 0;
					j = 0;
					i++;

				}
				while (*ptr == 0x20 && *ptr != 0)
					ptr++;

				if (isEmpty)
					isEmpty = false;
				continue;
			}
		}

		if (*ptr == '"')
		{
			quotes = !quotes;
			ptr++;
			continue;
		}

		if (j < 32)
		{
			*(argv[i] + j) = *ptr;
			j++;
		}
		ptr++;
	}

	return argv;
}

void shiftup(char* msg, uint8 index, char chr)
{
	uint8 i = strlen(msg) + 1;
	while (i > index)
	{
		*(msg + i) = *(msg + i - 1);
		i--;
	}
	*(msg + index) = chr;
}

void shiftdown(char* msg, uint8 index)
{
	uint8 len = strlen(msg);
	for (uint8 i = index; i < len; i++)
	{
		*(msg + i) = *(msg + i + 1);
	}
}

uint64 StringToHex(const char* str)
{
	uint64 ret = 0;
	const char* ptr = str;

	while (*ptr != 0)
	{
		if (*ptr >= '0' && *ptr <= '9')
		{
			ret += *ptr - 0x30;
		}
		if (*ptr >= 'a' && *ptr <= 'f')
		{
			ret += *ptr - 0x57;
		}
		if (*ptr >= 'A' && *ptr <= 'F')
		{
			ret += *ptr - 0x37;
		}

		ret = ret << 4;
		ptr++;
	}

	ret = ret >> 4;
	return ret;
}

uint64 StringToInt(const char* str)
{
	uint64 ret = 0;
	const char* ptr = str;

	while (*ptr != 0)
	{
		if (*ptr >= '0' && *ptr <= '9')
		{
			ret += *ptr - 0x30;
		}

		ret = ret * 10;
		ptr++;
	}

	ret = ret / 10;
	return ret;
}

char hexToSintrgOutout[128];
template<typename T>
const char* HexToString(T value)
{
	T* valPtr = &value;
	uint8* ptr;
	uint8 temp;
	uint8 size = (sizeof(T)) * 2 - 1;
	uint8 i;
	for (i = 0; i < size; i++)
	{
		ptr = ((uint8*)valPtr + i);
		temp = ((*ptr & 0xf0) >> 4);
		hexToSintrgOutout[size - (i * 2 + 1)] = temp + (temp > 9 ? 55 : 48);
		temp = ((*ptr & 0x0f));
		hexToSintrgOutout[size - (i * 2 + 0)] = temp + (temp > 9 ? 55 : 48);
	}
	hexToSintrgOutout[size + 1] = 0;
	return hexToSintrgOutout;
}

const char* HexToString(uint8 value) { return HexToString<uint8>(value); }
const char* HexToString(uint16 value) { return HexToString<uint16>(value); }
const char* HexToString(uint32 value) { return HexToString<uint32>(value); }
const char* HexToString(uint64 value) { return HexToString<uint64>(value); }
const char* HexToString(char value) { return HexToString<char>(value); }
const char* HexToString(short value) { return HexToString<short>(value); }
const char* HexToString(int value) { return HexToString<int>(value); }
const char* HexToString(long long value) { return HexToString<long long>(value); }

char integerToStringOutput[128];
template<typename T>
const char* IntToString(T value)
{
	uint8 isNegative = 0;

	if (value < 0)
	{
		isNegative = 1;
		value *= -1;
		integerToStringOutput[0] = '-';
	}

	uint8 size = 0;
	uint64 sizeTester = (uint64)value;
	while (sizeTester / 10 > 0)
	{
		sizeTester /= 10;
		size++;
	}

	uint8 index = 0;
	uint64 newValue = (uint64)value;
	while (newValue / 10 > 0)
	{
		uint8 remainder = newValue % 10;
		newValue /= 10;
		integerToStringOutput[size - index + isNegative] = remainder + 48;
		index++;
	}
	uint8 remainder = newValue % 10;
	integerToStringOutput[size - index + isNegative] = remainder + 48;
	integerToStringOutput[size + 1 + isNegative] = 0;
	return integerToStringOutput;
}

const char* IntToString(uint8 value) { return IntToString<uint8>(value); }
const char* IntToString(uint16 value) { return IntToString<uint16>(value); }
const char* IntToString(uint32 value) { return IntToString<uint32>(value); }
const char* IntToString(uint64 value) { return IntToString<uint64>(value); }
const char* IntToString(char value) { return IntToString<char>(value); }
const char* IntToString(short value) { return IntToString<short>(value); }
const char* IntToString(int value) { return IntToString<int>(value); }
const char* IntToString(long long value) { return IntToString<long long>(value); }

char floatToStringOutput[128];
const char* FloatToString(float value, uint8 decimals)
{
	char* intPtr = (char*)IntToString((int)value);
	char* floatPtr = floatToStringOutput;

	if (value < 0)
		value *= -1;

	while (*intPtr != 0)
	{
		*floatPtr = *intPtr;
		intPtr++;
		floatPtr++;
	}

	*floatPtr = '.';
	floatPtr++;

	float newValue = value - (int)value;

	for (uint8 i = 0; i < decimals; i++)
	{
		newValue *= 10;
		*floatPtr = (int)newValue + 48;
		newValue -= (int)newValue;
		floatPtr++;
	}

	*floatPtr = 0;

	return floatToStringOutput;
}