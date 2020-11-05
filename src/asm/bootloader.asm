org 0x7c00]

PROGRAM_SPACE equ 0x8000

jmp 0x0000:main

main:
cli
xor ax, ax
mov ds, ax 
mov es, ax 
mov ss, ax 
sti

mov [BOOT_DISK], dl

mov bp, 0x7c00
mov sp, bp

ReadDisk:
mov ah, 0x02
mov al, 127
mov ch, 0x00
mov dh, 0x00
mov cl, 0x02
mov bx, 0x0000
mov es, bx
mov bx, PROGRAM_SPACE
mov dl, [BOOT_DISK]
int 0x13
jc Failed
jmp PROGRAM_SPACE

Failed:
jmp $

BOOT_DISK: db 0

times 510-($-$$) db 0
dw 0xaa55