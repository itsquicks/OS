#include "CMOS.h"

uint8 second;
uint8 minute;
uint8 hour;
uint8 day;
uint8 month;
uint8 year;
uint8 ampmhour;
uint8 pm;

uint8 GetUpdateFlag() 
{
    outb(CMOS_ADDRESS, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

uint8 Get_RTC_Register(uint8 reg) 
{
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void Read_RTC() {

    uint8 last_second;
    uint8 last_minute;
    uint8 last_hour;
    uint8 last_day;
    uint8 last_month;
    uint8 last_year;

    while (GetUpdateFlag());

    second = Get_RTC_Register(0x00);
    minute = Get_RTC_Register(0x02);
    hour = Get_RTC_Register(0x04);
    day = Get_RTC_Register(0x07);
    month = Get_RTC_Register(0x08);
    year = Get_RTC_Register(0x09);

    do
    {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;

        while (GetUpdateFlag());

        second = Get_RTC_Register(0x00);
        minute = Get_RTC_Register(0x02);
        hour = Get_RTC_Register(0x04);
        day = Get_RTC_Register(0x07);
        month = Get_RTC_Register(0x08);
        year = Get_RTC_Register(0x09);

    } while ((last_second != second) ||
        (last_minute != minute) ||
        (last_hour != hour) ||
        (last_day != day) ||
        (last_month != month) ||
        (last_year != year));

    hour = HexAsInt(hour);

    hour += 2;
    if (hour > 24)
        hour -= 24;

    if (hour > 12)
        pm = true;
    else
        pm = false;

    ampmhour = hour;
    if (ampmhour > 12)
        ampmhour -= 12;

    hour = IntAsHex(hour);
}