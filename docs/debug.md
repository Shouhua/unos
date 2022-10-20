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
## qemu
```
sudo apt install qemu
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
```

## gdb + vscode
在vscode中调试时，可以在Debug Console中添加前缀-exec，然后跟上gdb调试命令
```-exec display/8xw $sp```