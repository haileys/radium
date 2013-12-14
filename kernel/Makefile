CFLAGS = \
	-Wall -Wextra -pedantic -Werror -nostdlib -nostdinc -fno-builtin \
	-nostartfiles -nodefaultlibs -fno-exceptions -ffreestanding \
	-fno-stack-protector -fno-pic -std=c99 -m32 -I inc

OBJECTS = \
	src/console.o \
	src/loader.o \
	src/main.o \
	src/string.o \

.PHONY: clean

radium.bin: linker.ld $(OBJECTS)
	@echo ld $@
	@ld -o $@ -T $^

%.o: %.c
	@echo cc $@
	@gcc -o $@ $(CFLAGS) -c $<

%.o: %.asm
	@echo nasm $@
	@nasm -felf -o $@ $<

clean:
	rm -f radium.bin $(OBJECTS)