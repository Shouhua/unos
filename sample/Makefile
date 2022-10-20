ASMBIN := BootLoader.bin BootLoader2.bin

all: a.img
	bochs -q

a.img: $(ASMBIN)
	gdd if=BootLoader.bin of=a.img bs=512 conv=notrunc count=1
	gdd if=BootLoader2.bin of=a.img bs=512 skip=1 conv=notrunc count=1
%.bin: %.asm
	nasm -f bin $< -o $@