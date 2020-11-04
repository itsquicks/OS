#pragma once
#include "Typedefs.h"

void outb(uint16 port, uint8 val);
uint8 inb(uint16 port);
void outw(uint16 port, uint16 val);
uint8 inw(uint16 port);