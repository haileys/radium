.PHONY: clean kernel/radium.bin

ifeq ($(shell uname),Darwin)
$(error Cowardly refusing to run on Mac OS X)
endif

all: hdd.img

hdd.img: hdd.base.img boot/grub/menu.lst kernel/radium.bin user/init.bin
	cp $< $@
	MTOOLSRC=mtoolsrc mcopy boot/grub/menu.lst C:/boot/grub/menu.lst
	MTOOLSRC=mtoolsrc mcopy kernel/radium.bin  C:/boot/radium.bin
	MTOOLSRC=mtoolsrc mcopy user/init.bin      C:/init.bin

hdd.base.img: hdd.base.img.gz
	gzip -dc $< > $@

clean:
	rm -f hdd.img hdd.base.img
	make -C kernel clean

kernel/radium.bin:
	make -C kernel radium.bin

user/init.bin:
	make -C user init.bin
