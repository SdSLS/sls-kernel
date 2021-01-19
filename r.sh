as --32 boot.s -o boot.o
gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ld -m elf_i386 -T linker.ld kernel.o boot.o -o Sls-OS.bin -nostdlib
grub2-file --is-x86-multiboot Sls-OS.bin
mkdir -p isodir/boot/grub
cp Sls-OS.bin isodir/boot/Sls-OS.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub2-mkrescue -o Sls-OS.iso isodir

qemu-system-x86_64 -cdrom Sls-OS.iso
