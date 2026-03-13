CC = i686-elf-gcc
AS = nasm
LNK = i686-elf-ld

OBJS = boot.o \
       kernel.o \
       drivers/screen/screen.o \
       drivers/keyboard/keyboard.o \
	   lib/string.o \
	   lib/shell.o \
	   arch/i386/io.o

OUTPUT = mokeos.bin

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(LNK) -T linker.ld -o $(OUTPUT) $(OBJS)

boot.o: boot.s
	$(AS) -f elf32 boot.s -o boot.o

drivers/screen/screen.o: drivers/screen/screen.c
	$(CC) -c drivers/screen/screen.c -o drivers/screen/screen.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

lib/string.o: lib/string.c
	$(CC) -c lib/string.c -o lib/string.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

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

