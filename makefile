#mokeOS beta!!
CC = i686-elf-gcc
AS = nasm
LNK = i686-elf-ld

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

OBJS = boot.o \
       kernel.o \
	   drivers/screen/screen.o \
	   drivers/keyboard/keyboard.o \
	   arch/i386/io.o \
	   lib/string/string.o \
	   lib/shell/shell.o \
	   drivers/hardware/hardware.o

OUTPUT = mokeos.bin

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LNK) -T linker.ld -o $(OUTPUT) $(OBJS)

boot.o: boot.s
	$(AS) -f elf32 boot.s -o boot.o

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OUTPUT)
	find . -name "*.o" -type f -delete