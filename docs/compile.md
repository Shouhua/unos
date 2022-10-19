## 预编译器cpp, 预编译和编译cc1，汇编器as，链接器ld
## 预编译
```
gcc -E hello.c -o hello.i
# 或者
cpp hello.c > hello.i
```
# 编译
```
gcc -S hello.c -o hello.s
# 有的平台把C语言的预编译和编译合成一个命令cc1
cc1 hello.c
```
# 汇编
```
gcc -c hello.c -o hello.o
#或者
as hello.s -o hello.o
```
# 链接
```
ld -static crt1.o crti.o crtbeginT.o hello.o -start-group -lgcc -lgcc_eh -lc-end-group crtend.o crtn.o
```