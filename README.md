# PotatOS
My 64 bit hobby toy kernel.

![Commit activity badge](https://img.shields.io/github/commit-activity/m/UnmappedStack/PotatOS/main?style=plastic)
![Top language badge](https://img.shields.io/github/languages/top/UnmappedStack/PotatOS?style=plastic&label=C)
![Lines of code badge](https://tokei.rs/b1/github/UnmappedStack/PotatOS)

![Screenshot of PotatOS](/screenshots/screenshot4.png)

PotatOS is a project I have been working on after I ended my previous project, [SpecOS](https://github.com/jakeSteinburger/SpecOS), due to the project being quite broken and having code left from when I knew literally nothing about kernel development. PotatOS is a rework now that I understand more, and while it's still not perfect, I can confidently say that it's already a lot better.

This is an x86_64, SMP-aware, higher-half, monolithic (perhaps modular in the future) kernel written in C, with a lettered Windows-style VFS (C:/, D:/ etc.) but using Linux style devices. It uses `spawn()` rather than `fork()`/`exec()`, and uses an event queue for handling events in user programs, rather than signals. It does *not attempt* to be binary compatible with other operating systems, and is neither UNIX-like nor DOS-like, but rather a blend of design aspects which I personally like. For this reason, a lot of it's userspace components will be completely written from scratch rather than ported.

I have a Discord server for PotatOS where I will share most updates, and you can also get help with your own OSDev project. You can join [here](https://discord.gg/hPg9S2F2nD).

## Special Thanks
The OS development community is a particularly helpful one, and there are a few people from my server who have been particularly helpful who I'd like to point out. I'm glad that they are all much smarter than me and I learn from them all the time! (This list is in no particular order.)

- [Dcraftbg](https://github.com/Dcraftbg) is an awesome guy who has helped me with a ton, especially when it comes to paging. He came up with the algorithm that I use to calculate page level indexes, he helped me with debugging paging quite a lot, and generally explained it to me really well, as well as explaining some VFS concepts to me. He also works on [MinOS](https://github.com/Dcraftbg/MinOS), so definitely check that out :D
- [Bananymous](https://github.com/Bananymous) helped me especially in the early stages of OSDev when I was working on SpecOS, and from that I've been able to make PotatOS so much better earlier on, so thank you! In the early days of SpecOS I basically completely relied on him xD. He also wrote an amazing kernel which is mostly POSIX compatible, [Banan-OS](https://github.com/Bananymous/banan-os).
- [AProgrammablePhoenex](https://github.com/AProgrammablePhoenix) (aka. Alex) has just been generally very supportive of my project, and has left many a 🌳 emoji as I rant about updates on PotatOS. Not to mention he was amazingly helpful when I was doing context switch, and his bug finds were the final steps to userspace support for me! He also is writing an OS named after everybody's fav fruit, [Cocos](https://github.com/AProgrammablePhoenix/Cocos) :)

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
- [X] Userspace support
- [ ] SMP
    - [X] Initialisation
    - [X] Spinlocks
    - [ ] Use with scheduler
    - [ ] Mutexes + Semaphores
- [ ] Dynamic ELF linking

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
- [X] APIC
- [X] PIT
- [X] Framebuffer
- [X] PS/2 Keyboard
- [X] LAPIC timer
- [ ] ACPI
- [ ] NVMe
- [ ] USB
- [ ] Some NIC driver

### Networking
Uhh I have no idea what this involves, I haven't done networking before. I'll see :D

### Userspace components
- [X] Some sorta libc
- [X] A userspace shell
- [ ] Some simple shell commands
- [ ] DOOM!
- [ ] Basic GUI/window server + components
- [ ] Terminal emulator
- [ ] Text editor
- [ ] Port TCC
- [ ] Port Python?

## Contributions
I'm currently not open to pull requests in most cases. If there's something specific that you *really* want to implement, you can open an issue and ask to be assigned to it *before* you begin. This may be changed in the future.

If you do contribute, it cannot contain code from any other licenses or contain non-original code without it being specified in a comment at the top of the file. If you do this, you also agree to your contribution being under the same license as the rest of the project, as seen below.

## License
This project is under the Mozilla Public License 2.0. See more information in the [LICENSE](https://github.com/jakeSteinburger/PotatOS/blob/main/LICENSE) file.