SOURCE :=BootLoader.asm BootLoader2.asm
TARGET :=$(SOURCE:.asm=.bin)

.PHONY: bochs a.img %.bin
bochs: a.img
	bochs -q	

a.img: ${TARGET}
	dd if=$< of=$@ bs=512 count=1 conv=notrunc
	dd if=BootLoader2.bin of=$@ seek=1 bs=512 count=5 conv=notrunc

%.bin: %.asm
	nasm -f bin $< -o $@	

clean:
	rm *.bin
