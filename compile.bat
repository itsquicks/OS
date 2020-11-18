nasm src/asm/bootloader.asm -f bin -i src/asm -o bootloader.bin

nasm src/asm/ExtendedProgram.asm -f elf64 -i src/asm -o ExtendedProgram.o

cmake CMakeLists.txt -G "Unix Makefiles" -D CMAKE_CXX_COMPILER=x86_64-elf-gcc -D CMAKE_C_COMPILER=x86_64-elf-gcc

make -f "Makefile"

copy /b bootloader.bin+Kernel.bin os.bin

dd if=/dev/zero of=os.img bs=512 count=128
dd if=os.bin of=os.img conv=notrunc

qemu-system-x86_64 -hda os.img -hdb hd.iso

pause