#pragma once
#include "../Typedefs.h"
#include "../IO.h"
#include "../Utils.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define century 20

extern uint8 second;
extern uint8 minute;
extern uint8 hour;
extern uint8 day;
extern uint8 month;
extern uint8 year;
extern uint8 ampmhour;
extern uint8 pm;

void Read_RTC();