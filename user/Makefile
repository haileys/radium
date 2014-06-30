CFLAGS = \
	-Wall -Wextra -pedantic -Werror -nostdlib -nostdinc -fno-builtin \
	-nostartfiles -nodefaultlibs -fno-exceptions -ffreestanding \
	-fno-stack-protector -fno-pic -std=c99 -m32 -masm=intel \
	-I ../api -fleading-underscore

all: init.bin

%.bin: crt0.o crt1.o %.o
	@echo ld $@
	@ld -o $@ -T linker.ld $^

crt0.o: crt0.asm
	@echo nasm $@
	@nasm -felf -o $@ $<

%.o: %.c
	@echo cc $@
	@gcc -o $@ $(CFLAGS) -c $<
