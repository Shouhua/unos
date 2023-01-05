/*
	主要测试dynamic link
	1. gcc helloworld.c -no-pie -fno-pie -g -o helloworld
	2. readelf -SW helloworld # 查看所有secionts，查看.plt, .got, .got.plt
	.got.plt主要是存储外部函数
	.got存储所有外部变量和函数，.got和.got.plt有重叠部分(overlap)
	3. gdb helloworld
	b *(&main+9)
	b *(&main+19)
	disassembly main
	disassembly 'puts@plt' #或者 p /x *(void**)0x404000
	查看jmp地址，发现就是当前rip下个指令地址，这是第一次，第二次got.plt里面就被复写了
	当第二次读取的时候发现puts@plt.got是另外一个地址了
*/
#include <stdio.h>

int main() {
	printf("Hello World\n");
	printf("Hello World Again\n");
	return 0;
}
