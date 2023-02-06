## qemu
```bash
# ubuntu中qemu是一个空包(dummy pakcage)
sudo apt install qemu-system-x86
```
```bash
BUILD_DIR := build
QEMU_MON_PORT := 55555

all:
	mkdir -p $(BUILD_DIR)
	nasm -f elf -o $(BUILD_DIR)/kernel.o loader.s
	ld -T link.ld -melf_i386 $(BUILD_DIR)/kernel.o -o iso/boot/kernel.bin
	grub-mkrescue -o $(BUILD_DIR)/os.iso iso
	bochs -q

debug:
	@nasm -f elf -g -o $(BUILD_DIR)/kernel.o loader.s
	@ld -T link.ld -melf_i386 $(BUILD_DIR)/kernel.o -o ${BUILD_DIR}/kernel.bin
	@cp ${BUILD_DIR}/kernel.bin iso/boot/
	@grub-mkrescue -o $(BUILD_DIR)/os.iso iso
	@i686-elf-objcopy --only-keep-debug ${BUILD_DIR}/kernel.bin ${BUILD_DIR}/kernel.debug
	@qemu-system-i386 -s -S -cdrom $(BUILD_DIR)/os.iso -monitor telnet::$(QEMU_MON_PORT),server,nowait &
	@sleep 1
	@gnome-terminal -- "telnet 127.0.0.1 $(QEMU_MON_PORT)"
	@gdb -s $(BUILD_DIR)/kernel.bin -ex "target remote localhost:1234"
	#@gdb -s $(BUILD_DIR)/kernel.debug -ex "target remote localhost:1234"
```

## bochs
### [bochs command line debug](https://bochs.sourceforge.io/doc/docbook/user/internal-debugger.html)
- disassembly
```shell
disasm 0x7c00 0x7c16 
# or
u 0x7c00 0x7c80
```
- print memory content
``` shell
# x /nuf address (n count, u unit size, f print format)
x /16wx 0x7c00 16个word
x /20bx es:di 20个byte
# print physical memeory
xp /20bx es:di 20个byte 
```
- ```trace on``` 每走一步打印当前和下一步的汇编代码
- information
```shell
# cpu registries and eflags 
reg
# segements
sreg
```

### [bochs stub](https://bochs.sourceforge.io/doc/docbook/user/compiling.html)
```shell
# --enable-gdb-stub和--enable-debug冲突
# 使用debug模式
./configure --enable-debugger --with-sdl2 --enable-usb-xhci --enable-raw-serial --enable-usb --enable-pcidev --enable-cdrom --enable-plugins --enable-debugger-gui --with-x11
# 使用gdb-stub模式
./configure --enable-usb-xhci --enable-raw-serial --enable-usb --enable-pcidev --enable-cdrom --enable-plugins --enable-gdb-stub
make
sudo make install
sudo make uninstall
make clean
```
## gdb
gdb中使用“x”命令来打印内存的值，格式为“x/nfu addr”。含义为以f格式打印从addr开始的n个长度单元为u的内存值。参数具体含义如下：
a）n：输出单元的个数。
b）f：是输出格式。比如x是以16进制形式输出，o是以8进制形式输出,等等。
c）u：标明一个单元的长度。b是一个byte，h是两个byte（halfword），w是四个byte（word），g是八个byte（giant word)。
```shell
gcc -g -o printf printf.c
gdb printf
run

help x

b _start
b elf.c:10
b elf.c:main
break 10 # linenumber, function name
b *0x1000af
info breakpoints(i b)

info proc #拿到当前执行的process id
info register(r) rsp


print 0x7fffffffdde8 # p 0x7fffffffdde8
x /16xb 0x7fffffffdde8
x /20i 0x12345 # 打印20条汇编指令
where
frame
step (finish/fin)
p /x *array@len
list /l/l-/l+/ l 1,10

layout [asm/src]
focus cmd # 在命令行中可以使用上下左右键检索
focus src
CTRL-X a # 推出layout模式

set disassembly-flavor intel
starti set breakpoint when first start program
start 执行程序至main主函数的起始位置停下
run 执行程序直到遇到断点

ptype 0x123 #打印当前类型
ptype /o #包括各个type的大小
```
https://fasterthanli.me/series/making-our-own-executable-packer/part-2
malloc底层也是使用mmap分配内存的
pmap
cat /proc/1234/maps
ls -i file # print inode of file
sudo debugfs -R "ncheck 3291229" /dev/sda1

man -k "^printf$"
1: user command
2: system calls
3: library calls

strace
```shell
strace -e trace=mmap,munmap build/bin/elk build/bin/hello-dl
```
lstrace
ldd test
pidof sth

real name
soname(shared object name)
libc.so.6 > libc-2.31.so

-fpie -fpic ???
gcc hello.c -lm -o hello
-lm必须出现在源文件后面，不然会报错

gcc默认使用延迟加载，所有可以实验初次运行到main时候，外部依赖没有值，但是调用时，plt.got更新

gcc -fpic -c source1.c source2.c
gcc -fpic source1.0 source2.0 -shared -Wl,-soname,libsource.so.1 -o libsource.so.1.1

LD_LIBRARY_PATH

gdb hello

.plt -> .got.plt -> .plt -> _dl_runtime_resolve(link_map, reloc_index) -> write .got.plt
https://ypl.coffee/dl-resolve/

## gdb + vscode
在vscode中调试时，可以在Debug Console中添加前缀-exec，然后跟上gdb调试命令
```-exec display/8xw $sp```
```json
// task.json
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "shell",
			"label": "C/C++: gcc build active file",
			"command": "/usr/bin/gcc",
			"args": [
				"-fdiagnostics-color=always",
				"-m32",
				"-g",
				"${file}",
				"-o",
				"${fileDirname}/${fileBasenameNoExtension}.out"
			],
			"options": {
				"cwd": "${fileDirname}"
			},
			"problemMatcher": [
				"$gcc"
			],
			"group": {
				"kind": "build",
				"isDefault": true
			},
			"detail": "compiler: /usr/bin/gcc"
		}
	]
}
```
```json
// launch.json
{
	"name": "gcc - Build and debug active file",
	"type": "cppdbg",
	"request": "launch",
	"program": "${fileDirname}/${fileBasenameNoExtension}.out",
	"args": [],
	"stopAtEntry": false,
	"cwd": "${fileDirname}",
	"environment": [],
	"externalConsole": false,
	"MIMode": "gdb",
	"setupCommands": [
			{
					"description": "Enable pretty-printing for gdb",
					"text": "-enable-pretty-printing",
					"ignoreFailures": true
			},
			{
					"description": "Set Disassembly Flavor to Intel",
					"text": "-gdb-set disassembly-flavor intel",
					"ignoreFailures": true
			}
	],
	"preLaunchTask": "C/C++: gcc build active file",
	"miDebuggerPath": "/usr/bin/gdb"
}
```