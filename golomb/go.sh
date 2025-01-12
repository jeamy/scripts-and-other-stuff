#/bin/bash
#nasm golomb2024.asm  -felf -g -Fdwarf
#gcc -g -m32 -nostdlib -static -o golomb2024 golomb2024.o

nasm -f elf32 golomb2024.asm
ld -m elf_i386 -o golomb2024 golomb2024.o
./golomb2024

