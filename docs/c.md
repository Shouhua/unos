 ## printf
 ```c
// 1. 堆栈问题: x86参数传递是通过函数栈方式传递的，x86_64前6个传参使用寄存器，后面才使用栈方式
// 2. 使用2级指针获取可变参数, 详见printf实现代码
void my_printf(const char *format, int num) {
	char **arg = (char **) &format;
	printf("%s\n", *arg);
	arg++;
	printf("%d", *((int *) arg++));
}
 ```
 ## pipe
 例子可以参见sample/elf_parse/elk.c
 1. file descriptor只是给用户态使用的数字，真正连接到kernel中的实例，所以一个实例可以有多个描述符对应。
 2. 如果直接在程序中exec程序，会覆盖程序代码段，后面的程序不会执行，所以需要fork在子程序中exec。
 3. pipe是生成2个file descriptor对应pipe的输入与输出
 4. fork后，父与子公用代码段，copy数据段，包括对应的file descriptor，除非使用了close on exec。父程序中新建的pipe，子程序中也有一套，但是close了不影响父中的file descriptor，这就是最上面说的。
 5. dup/dup2用法可以查看man，主要用于新建一个file descriptor指向与old一样的内核对象，用法不一样，常见如下：
 ```c
 // dup，先删除stdout，然后复制，会使用stdout，此时stdout和filedes[1]都指向相同内核对象
 close(STDOUT_FILENO)
 dup(fildes[1])

 // dup2,可以指定file descriptor
 dup2(fildes[1], STDOUT_FILENO)
 ```
 ## strcpy/memcpy
 memcpy内存级别的copy，size多少就拷贝多少；strcpy会考虑内存中的terminator，如果碰到了，就停止了

 ## 内存相关
 mmap
 malloc
 pmap
 mprotect 注意第一个参数起始地址必须是PAGE_SIZE对齐的，addr&0xfff，不然返回-1，errno=22
 一定要注意指针加减，会根据指针类型加减，否则就要将地址转化成数值类型进行运算