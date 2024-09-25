if [ ! -d "limine" ]; then
    git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1
fi

# compile the bootloader & the kernel

make -C limine

make

# compile/assemble userspace components

gcc -ffreestanding -nostdlib -fno-stack-protector -fno-stack-check -I libc -e main -o ramdiskroot/testuser userspace/test/main.c -g

# set up the initial ramdisk

tar --create --format=ustar --file=initrd ramdiskroot

# set up the sysroot

mkdir -p sysroot

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

qemu-system-x86_64 potatos.iso -serial stdio --no-reboot --no-shutdown -smp 5 -accel kvm -M smm=off -d int
