/**
 * gcc -m32 -o printf printf.c
 * gdb printf
 * break 10
 * x/16xb arg
 * x86参数传递是通过函数栈方式传递的，x86_64前6个传参使用寄存器，后面才使用栈方式
 */
#include <stdio.h>

// void my_printf(const char *format, ...) {
void my_printf(const char *format, int num) {
	char **arg = (char **) &format;
	printf("%s\n", *arg);
	arg++;
	printf("%d", *((int *) arg++));
}

void main() {
	my_printf("helo: %d", 3);
}