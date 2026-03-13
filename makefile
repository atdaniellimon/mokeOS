CC = i686-elf-gcc
AS = nasm
LNK = i686-elf-ld

# Archivos
OBJS = boot.o kernel.o
OUTPUT = mokeos.bin

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LNK) -T linker.ld -o $(OUTPUT) $(OBJS)

boot.o: boot.s
	$(AS) -f elf32 boot.s -o boot.o

kernel.o: kernel.c
	$(CC) -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

clean:
	rm -f *.o $(OUTPUT) mokeos.iso