#!/bin/bash
set -e

if [ ! -d "limine" ]; then
    git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
fi

# compile the bootloader & the kernel

echo "Building bootloader..."
make -C limine

echo "Building kernel..."
make

echo "Building libc..."
make clean -C libc
make -C libc
nasm -felf64 -o libc/target/crt0.o libc/internal/crt0.asm

# compile/assemble userspace components
echo "Building userspace applications..."
mkdir -p ramdiskroot/exec
mkdir -p obj/user
gcc -ffreestanding -nostdlib -I libc/include -c userspace/shell.c -o obj/user/shell.o -g
gcc -ffreestanding -nostdlib -I libc/include -c userspace/helloworld.c -o obj/user/helloworld.o -g

echo "Linking user applications..."
gcc -o ramdiskroot/exec/shell libc/target/crt0.o obj/user/shell.o libc/target/spudlibc.a -nostdlib -ffreestanding -g
gcc -o ramdiskroot/exec/helloworld libc/target/crt0.o obj/user/helloworld.o libc/target/spudlibc.a -nostdlib -ffreestanding -g

# set up the initial ramdisk
tar --create --file=initrd --format=ustar -C ramdiskroot exec sys
# set up the sysroot

mkdir -p sysroot
cp media/limine_wallpaper.jpeg sysroot
mkdir -p sysroot/boot
mv initrd sysroot/boot
cp -v bin/potatos sysroot/boot/
mkdir -p sysroot/boot/limine
cp -v src/limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin \
      limine/limine-uefi-cd.bin sysroot/boot/limine/

mkdir -p sysroot/EFI/BOOT
cp -v limine/BOOTX64.EFI sysroot/EFI/BOOT/
cp -v limine/BOOTIA32.EFI sysroot/EFI/BOOT/

xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        sysroot -o potatos.iso

./limine/limine bios-install potatos.iso

# run in qemu

qemu-system-x86_64 potatos.iso -serial stdio --no-reboot --no-shutdown -smp 5 -accel kvm -monitor telnet:127.0.0.1:8000,server,nowait
