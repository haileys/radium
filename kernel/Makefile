CFLAGS = \
	-Wall -Wextra -pedantic -Werror -nostdlib -nostdinc -fno-builtin \
	-nostartfiles -nodefaultlibs -fno-exceptions -ffreestanding \
	-fno-stack-protector -fno-pic -std=c99 -m32 -I inc -masm=intel

OBJECTS = \
	src/console.o \
	src/idt.o \
	src/isrs.o \
	src/gdt.o \
	src/gdt_helper.o \
	src/loader.o \
	src/main.o \
	src/panic.o \
	src/panic_helper.o \
	src/string.o \

.PHONY: clean

radium.bin: linker.ld $(OBJECTS) script/embed_symbol_table.rb
	@echo ld $@
	@ld -o $@ -T linker.ld $(OBJECTS)
	ruby script/embed_symbol_table.rb

%.o: %.c
	@echo cc $@
	@gcc -o $@ $(CFLAGS) -c $<

%.o: %.asm
	@echo nasm $@
	@nasm -felf -o $@ $<

clean:
	rm -f radium.bin $(OBJECTS)
