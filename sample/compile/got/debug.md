```c
#include <stdio.h>

int main() {
	printf("Hello World\n");
	printf("Hello World Again\n");
	return 0;
}
```
gcc hello.c -fno-PIE -no-PIE -m32 -g -o hello
gdb hello
pwndbg(https://github.com/pwndbg/pwndbg)
cd pwndbg
./setup.sh 会在~/.gdbinit文件中添加，如果不想gdb使用pwndbg，可以使用#注释就行
pwndbg inspect elf：
```got, plt, gotplt```
gdb command:
```shell
x /10s 0x123456
x /10i 0x123456
x /10wx 0x123456
si
b *(&main+9)
b *(&main+19)
disassembly main
disassembly 'puts@plt' #或者 p /x *(void**)0x404000
```
其他还在探索中
```shell
readelf -h file header
readelf -l program header
readelf -r relocation section
readelf -d dynamic header
readelf -SW sections
readelf -sDW dynamic symbols
readelf -x .dynstr hello print .dynstr section like hexdump
readelf -p .dynstr hello print .dynstr section
readelf -R .rel.plt

objdump -s -d -j .plt -j .got.plt hello
```
**inspect elf各种头，比如file header, dynamic header可以获取各种section item的size，如果要查看各种数据结构，可以使用man 5 elf查看**

push 'helloworld' 0x8048008
call [plt item 'puts'] 0x8049050
jump .got.plt 'puts' 0x804c004(这个就是后面要修改的地址*0x804c004)
.got.plt里面保存着0x8049056，jump 0x8049056(puts.plt)
push 8 (relacation index .rel.plt[8])
jmp 0x8049030(.plt最开始，准备进入_dl_runtime_resolve)

**GOT global offset table, 在动态链接中，GOT代表.got.plt**
push link_map(0x804bff8)(.got.plt+4)(获取.dynamic内容，找到.dynstr, .dynsym, .rel.plt)
jmp .got.plt+8(_dl_runtime_resolve -> fixup)

_dl_runtime_resolve(link_map, dynsym_index)
link_map address = 0xf7ffda20
link_map[3] = .dynamic address = 0x0x804bf00
.dynamic[.dynstr 17] = 0x080482d0
.dynamic[.dynsym 19] = 0x08048260
.dynamic[.rel.plt 33] = 0x080483b4

.rel.plt+8找到8个字节(r_offset, r_info)
r_offset = 0x804c004
r_info = 0x00000307
r_symindex = r_info >> 8 = 3
r_type = r_info & 0xff = 7

size of .dynsym item = 16 bytes
.dynsym[3].st_name = .dynstr index =  0x22(4 bytes)
到这里找到'puts', 然后在库中找到对应的地址填入到.rel.plt中r_offset指定的地址中(0x804c004)，实际上，填入的实际地方是GOT中puts项，即
*0x804c004 = 0xf7fc03e0