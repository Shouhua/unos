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
break 10 # linenumber, function name
info breakpoints
print 0x7fffffffdde8 # p 0x7fffffffdde8
x /16xb 0x7fffffffdde8
b *0x1000af
where
frame
step (finish/fin)
p /x *array@len
list /l/l-/l+/ l 1,10
layout [asm/src]
```

## gdb + vscode
在vscode中调试时，可以在Debug Console中添加前缀-exec，然后跟上gdb调试命令
```-exec display/8xw $sp```

```shell
# 一个文件/home/username/project/folder/file.ext 在编辑器中打开
# 一个目录/home/username/project 作为你的根目录
${workspaceFolder} 当前工作根目录 /home/username/project
${workspaceFolderBasename} 当前文件父目录 project
${file} 当前打开的完整文件名 file.ext
${relativeFile} 当前打开文件相对当前根目录的路径文件名 folder/file.ext
${relativeFileDirname} 当前打开文件相对当前根目录的路径 folder
${fileBasename} 当前打开的文件名 file.ext
${fileBasenameNoExtension} 当前打开文件名不包括扩展 file
${fileDirname} 当前打开文件的路径 /home/username/project/folder
${fileExtname} 当前打开文件的扩展名 ext
${cwd} 启动时task工作的目录
${lineNumber} 光标所在行
${selectedText} 编辑器中所选文本
${execPath} code.exe所在目录路径
${defaultBuildTask} 默认build task的名字
```
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