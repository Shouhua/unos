#include <stdio.h>

void main(int argc, char *argv[], char *envp[])
{
	printf("argc: %d\n", argc);
	printf("argv[0]: %s\n", argv[0]);
	printf("envp[0]: %s\n", envp[0]);
}