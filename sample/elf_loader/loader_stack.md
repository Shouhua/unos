### loader解释
https://github.com/malisal/loaders
本文件夹中loader_temp.*为原始文件，loader.*为新文件

execve
.interp (fill the GOT and PLT table)
_start

stack content:
...(high)
local variables of main
saved registers of main
return address of main
argc
argv
envp
stack from startup code
----------------要研究的堆栈开始--------------------------
详细可以参考：https://lwn.net/Articles/631631/
argc
argv pointers
NULL that ends argv[]
environment pointers
NULL that ends envp[]
ELF Auxiliary Table
argv strings
environment strings
program name
NULL(low)

https://www.mgaillard.fr/2021/04/15/load-elf-user-mode.html
open file
read file
check elf file validation
apply memory by using mmap (sys/mmap), because need to set memory attributes like read/write/private etc
memset memory

loop program headers
check type PT_LOAD
check filesz <= memsz
check filesz > 0
copy PT_LOAD content to mmap area
set memory attribute

Relocate global dynamic symbols
loop section with SHT_REL type
loop relocation section items
loop relation item 
r_offset, r_info(r_dynsym_index, r_type)
find global symbol string: relocation item -> dynamic symbol[r_dynsym_index].st_name -> .dynstr+st_name
resolve string: 
handle = dlopen("libc.so.6", RTLD_NOW);
real_addr = dlsym(handle, sym);
	
*relocation_item.r_offset = real_addr
实际例子中，比如puts，relocation里面的r_offset指向.got.plt中puts的对应项

find the main function in the symbol table and execute
查找.symtab(因为是本地main所以使用使用symtab，而不是dynsym)
.symtab -> sh_link 对应的.symstr section index
.symstr = src + shdr[.symstr section index].sh_offset
loop .symtab items
.symtab_item.st_name+.symstr == "main"
main_addr = .symtab_item.st_value + mmap_start_addr
main_addr(argc, argv, envp)

https://github.com/malisal/loaders/blob/master/elf/elf.c

get random bytes(16 bytes)
allocate stack space 8M(mmap)
load elf file to stack memory