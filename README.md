# PotatOS
My 64 bit hobby toy kernel.

PotatOS is a project I have been working on after I ended my previous project, [SpecOS](https://github.com/jakeSteinburger/SpecOS), due to the project being quite broken and having code left from when I knew literally nothing about kernel development. PotatOS is a rework now that I understand more, and while it's still not perfect, I can confidently say that it's already a lot better.

This is an x86_64, SMP-aware, higher-half, monolithic (perhaps modular in the future) kernel written in C, with a lettered Windows-style VFS (C:/, D:/ etc.) but using Linux style devices. It uses `spawn()` rather than `fork()`/`exec()`. It does *not attempt* to be binary compatible with other operating systems, and is neither UNIX-like nor DOS-like, but rather a blend of design aspects which I personally like. For this reason, a lot of it's userspace components will be completely written from scratch rather than ported.

I have a Discord server for PotatOS where I will share most updates, and you can also get help with your own OSDev project. You can join [here](https://discord.gg/hPg9S2F2nD).

## Usage
Clone the repository, make the `compile.sh` file executable, and run the compilation script. It will automatically run in the Qemu emulator.
```bash
git clone https://github.com/jakeSteinburger/PotatOS
cd PotatOS
chmod +x compile.sh
./compile.sh
```
If you want to run on real hardware, you can still use these same commands, and `potatos.iso` will appear in the project root. You can copy this onto a USB stick to boot like so:
```bash
sudo dd if=potatos.iso of=/dev/sdaX status=progress
```
Replace `/dev/sdaX` with the device path of the USB. Make sure that secure boot is off and legacy boot is on.

## Roadmap
### General
- [X] GDT
- [X] IDT
- [X] Exception handler
- [ ] Userspace support
- [ ] Dynamic ELF linking
- [X] SMP

### Memory
- [X] Physical memory allocator (bitmap)
- [X] Kernelspace heap
- [X] Paging
- [ ] Better buddy allocator
- [ ] On-demand paging

### File Systems
- [X] TempFS
- [X] VFS
- [X] Initial ramdisk (USTAR archive)
- [ ] ext2
- [ ] FAT32

### Drivers
- [X] Serial
- [X] PIC
- [X] PIT
- [X] Framebuffer
- [ ] ACPI
- [ ] NVMe
- [ ] PS/2 Keyboard
- [ ] USB
- [ ] Some NIC driver

### Networking
Uhh I have no idea what this involves, I haven't done networking before. I'll see :D

### Userspace components
- [ ] Some sorta libc
- [ ] DOOM!
- [ ] A shell + utilities
- [ ] Basic GUI/window server + components
- [ ] Terminal emulator
- [ ] Text editor
- [ ] Port TCC
- [ ] Port Python?

## License
This project is under the Mozilla Public License 2.0. See more information in the [LICENSE](https://github.com/jakeSteinburger/PotatOS/blob/main/LICENSE) file.