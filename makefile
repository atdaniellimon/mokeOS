CC = i686-elf-gcc
AS = nasm
LNK = i686-elf-ld

<<<<<<< Updated upstream
OBJS = boot.o \
       kernel.o \
       drivers/screen/screen.o \
       drivers/keyboard/keyboard.o \
	   lib/string.o \
	   lib/shell.o \
	   arch/i386/io.o
=======
UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)

ifeq ($(UNAME_S),Linux)
	OS_CFLAGS := -fno-stack-protector -fno-pie -fno-pic -mno-mmx -mno-sse -mno-sse2 -mno-80387 -msoft-float -fno-tree-vectorize
	ifneq ($(shell command -v i686-elf-gcc 2>/dev/null),)
		CC := i686-elf-gcc
		LNK := i686-elf-ld
		ARCH_CFLAGS := -march=i386
		ARCH_LDFLAGS :=
	else ifneq ($(shell command -v i686-linux-gnu-gcc 2>/dev/null),)
		CC := i686-linux-gnu-gcc
		LNK := i686-linux-gnu-ld
		ARCH_CFLAGS := -march=i386
		ARCH_LDFLAGS :=
	else
		CC := gcc
		LNK := ld
		ARCH_CFLAGS := -m32 -march=i386
		ARCH_LDFLAGS := -m elf_i386
	endif
else
	OS_CFLAGS :=
	CC := i686-elf-gcc
	LNK := i686-elf-ld
	ARCH_CFLAGS :=
	ARCH_LDFLAGS :=
endif

AS := nasm

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

OBJS = boot.o 						\
       kernel.o 					\
	   lib/string/string.o  		\
	   lib/shell/shell.o 			\
	   lib/shell/terminal.o 		\
	   lib/timer/timer.o 			\
	   lib/date/date.o 				\
	   lib/mokeUI/import.o			\
	   lib/malloc/mem.o				\
	   lib/syscall/syscall.o		\
	   lib/paging/paging.o			\
	   lib/elf/elf.o 				\
	   drivers/hardware/hardware.o 	\
	   drivers/vbe/vbe.o 			\
       drivers/vbe/font.o 			\
	   drivers/mouse/ps2.o 			\
	   drivers/screen/screen.o 		\
	   drivers/keyboard/keyboard.o 	\
	   arch/i386/gdt.o 				\
	   arch/i386/gdt_asm.o			\
	   arch/i386/idt.o 				\
       arch/i386/idt_asm.o 			\
	   arch/i386/io.o				\
	   arch/i386/tss.o				\
	   arch/i386/usermode.o 		\
	   debug/mfs/moke.o				
>>>>>>> Stashed changes

OUTPUT = mokeos.bin

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LNK) -T linker.ld -o $(OUTPUT) $(OBJS)

boot.o: boot.s
	$(AS) -f elf32 boot.s -o boot.o

<<<<<<< Updated upstream
drivers/screen/screen.o: drivers/screen/screen.c
	$(CC) -c drivers/screen/screen.c -o drivers/screen/screen.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

lib/string.o: lib/string.c
	$(CC) -c lib/string.c -o lib/string.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
=======
arch/i386/gdt_asm.o: arch/i386/gdt.asm
	$(AS) -f elf32 arch/i386/gdt.asm -o arch/i386/gdt_asm.o

arch/i386/idt_asm.o: arch/i386/idt.asm
	$(AS) -f elf32 arch/i386/idt.asm -o arch/i386/idt_asm.o

arch/i386/usermode.o: arch/i386/usermode.asm
	$(AS) -f elf32 arch/i386/usermode.asm -o arch/i386/usermode.o
>>>>>>> Stashed changes

lib/shell.o: lib/shell.c
	$(CC) -c lib/shell.c -o lib/shell.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

arch/i386/io.o: arch/i386/io.c
	$(CC) -c arch/i386/io.c -o arch/i386/io.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

drivers/keyboard/keyboard.o: drivers/keyboard/keyboard.c
	$(CC) -c drivers/keyboard/keyboard.c -o drivers/keyboard/keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -f *.o drivers/screen/*.o drivers/keyboard/*.o $(OUTPUT)

