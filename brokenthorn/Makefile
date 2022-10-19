all: a.img

a.img: Boot1.bin
	dd if=$< of=$@ bs=512k count=1 conv=notrunc

*.bin: *.asm
	nasm -f bin $< -o $@

bochs: a.img
	bochs -q