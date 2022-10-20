ASMBIN := BootLoader.bin BootLoader2.bin
UNAME_S := $(shell uname -s)
DD := dd
BOCHSRC := bochs_linuxrc
ifeq ($(UNAME_S), Darwin)
	DD := gdd	
	BOCHSRC := bochs_darwinrc
else
	DD := dd
	BOCHSRC := bochs_linuxrc
endif

.PHONY: all %.bin clean

all: a.img
	bochs -q -f $(BOCHSRC)

a.img: $(ASMBIN)
	$(DD) if=BootLoader.bin of=a.img bs=512 conv=notrunc count=1
	$(DD) if=BootLoader2.bin of=a.img bs=512 seek=1 conv=notrunc count=1
%.bin: %.asm
	nasm -f bin $< -o $@

clean:
	rm *.bin