#include "Syscalls.h"

void syscall_vga()
{
    uint64 service = 0;

    asm volatile ("movq %%rbx, %0" : "=b"(service));

    if (service == 1)
    {
        uint64 chr;
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(color));
        asm volatile ("movq %%rdx, %0" : "=d"(chr));

        Print((char)chr, (uint8)color);
        return;
    }

    if (service == 2)
    {
        uint64 str;
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(color));
        asm volatile ("movq %%rdi, %0" : "=D"(str));

        Print((char*)str, (uint8)color);
        return;
    }

    if (service == 3)
    {
        uint64 num;
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(color));
        asm volatile ("movq %%rdx, %0" : "=d"(num));

        Print(IntToString(num), color);
        return;
    }

    if (service == 4)
    {
        uint64 num;
        uint64 color;
        uint64 acc;

        asm volatile ("movq %%rcx, %0" : "=c"(color));
        asm volatile ("movq %%rdx, %0" : "=d"(num));
        asm volatile ("movq %%rdi, %0" : "=D"(acc));

        float f;
        memcpy(&f, &num, 4);

        Print(FloatToString(f, acc), color);
        return;
    }

    if (service == 5)
    {
        uint64 string;
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(color));
        asm volatile ("movq %%rdi, %0" : "=D"(string));

        PrintBack((char*)string, (uint8)color);
        return;
    }

    if (service == 6)
    {
        uint64 pos;

        asm volatile ("movq %%rcx, %0" : "=c"(pos));

        SetCursorPosition(pos);
        return;
    }

    if (service == 7)
    {
        uint64 color;
        asm volatile ("movq %%rcx, %0" : "=c"(color));

        ClearScreen(color);
        return;
    }

    if (service == 8)
    {
        uint64 start;
        uint64 len;
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(start));
        asm volatile ("movq %%rdx, %0" : "=d"(len));
        asm volatile ("movq %%rdi, %0" : "=D"(color));

        ClearScreen(start, len, color);
        return;
    }

    if (service == 9)
    {
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(color));

        PaintScreen(color);
        return;
    }

    if (service == 10)
    {
        uint64 start;
        uint64 len;
        uint64 color;

        asm volatile ("movq %%rcx, %0" : "=c"(start));
        asm volatile ("movq %%rdx, %0" : "=d"(len));
        asm volatile ("movq %%rdi, %0" : "=D"(color));

        PaintScreen(start, len, color);
        return;
    }

    if (service == 11)
    {
        asm volatile("movq %0, %%rcx" :: "c"((uint64)cursorPos));
        return;
    }
}