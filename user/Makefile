all: init.bin

%.bin: %.asm
	@echo nasm $@
	@nasm -f bin -o $@ $<
