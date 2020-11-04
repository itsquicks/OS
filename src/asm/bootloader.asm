org 0x7c00]

jmp 0x0000:main

main:

xor ax, ax
mov ds, ax 
mov es, ax 
mov ss, ax 

mov [BOOT_DISK], dl
PROGRAM_SPACE equ 0x8000

mov bp, 0x7c00
mov sp, bp

ReadDisk:
mov ah, 0x02
mov bx, PROGRAM_SPACE
mov al, 99
mov dl, [BOOT_DISK]
mov ch, 0x00
mov dh, 0x00
mov cl, 0x02
int 0x13
jc Failed
jmp PROGRAM_SPACE

Failed:
mov ah, 0x0e
mov al, 0x69
mov bl, 0x0a
mov bh, 0
int 0x10
jmp $

BOOT_DISK: db 0

times 446-($-$$) db 0
db 0x80
times 510-($-$$) db 0
dw 0xaa55