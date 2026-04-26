# mokeOS Beta

**A bare-metal graphical operating system with a window manager, filesystem, and app support — built entirely from scratch in C and Assembly.**

## Overview

mokeOS is a 32-bit x86 operating system that boots directly on real hardware or emulators (QEMU, Bochs). It features a fully functional GUI desktop environment with window management, a custom filesystem, dynamic font rendering, and the ability to load and run user applications — all without any external dependencies.

## Features

### Graphical Desktop
- Double-buffered VBE graphics at native resolution (1024x768)
- Window manager with draggable windows and title bars
- Custom cursor with multiple styles (arrow, I-beam, pointer)
- Rounded rectangles with border-radius support
- Multiple layered rendering with dirty rectangle optimization

### Typography
- Dynamic font system supporting multiple sizes (8px, 12px, 16px)
- 1-bit packed bitmap fonts generated from TrueType files
- Proportional character spacing (kerning)
- Runtime font switching

### Filesystem
- Custom MokeFS filesystem (FAT12-inspired)
- ATA disk driver with LBA addressing
- Hierarchical directory structure
- File listing, reading, and loading

### Application Support
- User-mode application loading via syscall interface (int 0x80)
- Apps run in ring 3 with proper memory isolation
- Cooperative multitasking with shell compositing
- Example Hello World app included

### Hardware Support
- PS/2 keyboard and mouse drivers
- RTC clock with time/date setting
- VESA BIOS Extensions (VBE) for graphics
- PIT timer for system ticks
- PCI bus enumeration

### Development Tooling
- **MokeTools** — GUI disk utility (Python/Qt6) for:
  - Font generation from TrueType → C headers
  - Binary injection into disk image
  - Disk image creation and formatting
  - Raw sector hex viewer

## Architecture

```
┌─────────────────────────────────────┐
│           User Applications         │  Ring 3
│    (hello.bin, future apps...)     │
├─────────────────────────────────────┤
│         Sherlock Shell/UI           │  Ring 3
│   Window Manager + Compositor      │
├─────────────────────────────────────┤
│            Syscall Layer            │  Ring 0
│         (int 0x80 handler)         │
├─────────────────────────────────────┤
│              Kernel                 │  Ring 0
│  ┌─────────┐ ┌──────────────────┐  │
│  │  Memory  │ │   Device Drivers  │  │
│  │  Manager │ │  (VBE, ATA, PS/2) │  │
│  └─────────┘ └──────────────────┘  │
│  ┌─────────┐ ┌──────────────────┐  │
│  │   GDT    │ │      MokeFS       │  │
│  │   IDT    │ │   (Filesystem)    │  │
│  └─────────┘ └──────────────────┘  │
├─────────────────────────────────────┤
│           Bootloader (boot.s)       │
│      Multiboot-compliant ELF       │
└─────────────────────────────────────┘
```

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Languages | C (GNU99), x86 Assembly (NASM) |
| Architecture | i386 (32-bit x86) |
| Build System | GNU Make, i686-elf cross-compiler |
| Binary Format | ELF → flat binary via linker script |
| Output | ISO 9660 (GRUB bootable) |
| Emulation | QEMU, VirtualBox, Bochs |
| Tooling | Python 3, PyQt6, Pillow |

## Building

### Requirements
- `i686-elf-gcc` cross-compiler
- `nasm` assembler
- `i686-elf-grub-mkrescue` (for ISO creation)
- Python 3 + PyQt6 + Pillow (for MokeTools)

### Build
```bash
    make
```

## Project Structure

```
mokeOS/
├── arch/i386/          # GDT, IDT, I/O ports
├── drivers/
│   ├── vbe/            # VESA graphics driver + fonts
│   ├── mouse/          # PS/2 mouse
│   ├── keyboard/       # PS/2 keyboard
│   └── hardware/       # CPU/RAM detection
├── lib/
│   ├── kernel/         # Syscall handler, graphics protocol
│   ├── shell/          # Window manager (Sherlock)
│   ├── MokeSDK/        # UI library
│   ├── mfs/            # MokeFS filesystem
│   ├── malloc/         # Kernel heap allocator
│   └── timer/          # PIT driver
├── debug/mfs/          # FS debug utilities
├── boot.s              # Multiboot bootloader
├── kernel.c            # Kernel entry point
├── linker.ld           # Memory layout
├── makefile            # Build system
├── i.sh                # Run script
└── MokeTools/          # Python disk utility
    ├── main.py
    ├── mfs_engine.py
    └── style.qss
```

## Why

Most developers never write code without an OS underneath. mokeOS was built to understand what happens from the moment the CPU powers on — from GDT/IDT setup and protected mode transitions to hardware I/O, memory management, and compositing graphics — all at the bare metal level.

## License

MIT