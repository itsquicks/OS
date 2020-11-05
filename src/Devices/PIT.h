#pragma once
#include "../Typedefs.h"
#include "../IO.h"

#define PIT_CHANNEL_0_PORT 0x40
#define PIT_COMMAND_PORT 0x43

void InitializePIT(uint32 freq);