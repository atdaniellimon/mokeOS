#mokeOS beta!!

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
	   lib/timer/timer.o 			\
	   lib/date/date.o 				\
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
	   arch/i386/io.o

OUTPUT = mokeos.bin

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LNK) $(ARCH_LDFLAGS) -T linker.ld -o $(OUTPUT) $(OBJS)

boot.o: boot.s
	$(AS) -f elf32 boot.s -o boot.o

arch/i386/idt_asm.o: arch/i386/idt.asm
	$(AS) -f elf32 arch/i386/idt.asm -o arch/i386/idt_asm.o

arch/i386/gdt_asm.o: arch/i386/gdt.asm
	$(AS) -f elf32 arch/i386/gdt.asm -o arch/i386/gdt_asm.o

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(ARCH_CFLAGS) $(OS_CFLAGS)

clean:
	rm -f $(OUTPUT)
	find . -name "*.o" -type f -delete