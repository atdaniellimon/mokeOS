# mokeOS Beta

A bare-metal x86 operating system built from scratch in C and Assembly.

## Overview

mokeOS is a minimal operating system targeting the i386 architecture. It boots directly on real hardware or emulators (QEMU, Bochs) with no dependencies on existing operating systems or runtime environments. Every component — from the bootloader to the kernel — is written by hand.

## Features

- Custom bootloader written in x86 Assembly
- Minimal C kernel running in protected mode
- Basic hardware drivers
- Custom standard library (`lib/`)
- Linker script for precise memory layout control
- Fully buildable with a single `make` command

## Project Structure

```
mokeOS/
├── arch/i386/       # x86-specific code (GDT, IDT, etc.)
├── drivers/         # Hardware drivers
├── lib/             # Minimal standard library
├── boot.s           # Bootloader (Assembly)
├── kernel.c         # Kernel entry point
├── linker.ld        # Linker script
└── makefile         # Build system
```

## Tech Stack

|Layer        |Technology                        |
|-------------|----------------------------------|
|Language     |C, x86 Assembly                   |
|Architecture |i386 (32-bit x86)                 |
|Build System |GNU Make, GCC cross-compiler      |
|Binary Format|ELF → raw binary via linker script|

## Building

Requirements: `gcc` cross-compiler targeting `i686-elf`, `nasm`, `make`.

```bash
make
```

This produces a bootable binary image.

## Running

```bash
./i.sh
```

## Why

Most developers never write code without an OS underneath. mokeOS was built to understand what actually happens before `main()` — interrupts, memory layout, protected mode, and hardware I/O at the bare metal level.

## License

MIT