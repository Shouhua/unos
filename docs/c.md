 - printf
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