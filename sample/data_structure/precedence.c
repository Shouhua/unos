/**
 * C语言优先级(Precedence)和相关性(Associativity)
 * https://stackoverflow.com/questions/859770/post-increment-on-a-dereferenced-pointer
 * https://www.cs.uic.edu/~i109/Notes/COperatorPrecedenceTable.pdf
 * 遇到的比如：
 * 1. while(*dst++ = *src++) *和++同样的优先级，right->left, 用于先判断然后dst，src在加1
 * 2. *addr+index 这个会导致错误，因为*优先级比+/-要高，先取值*addr，然后+index, 所以使用*(addr+index)
*/
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int arr[] = {100, 10, 1};
	int size = sizeof(arr)/sizeof(int);
	// int *arr = (int *)malloc(3);
	// *arr = 100;
	// *(arr+1)= 10;
	// *(arr+2)= 1;
	// int size = 3;
	printf("Wrong: ");
	for(int i=0; i<size; i++)
	{
		printf("%d\t", (*arr+i));
	}
	printf("\n");
	printf("Right: ");
	for(int i=0; i<size; i++)
	{
		printf("%d\t", (*(arr+i)));
	}
	printf("\n");
}