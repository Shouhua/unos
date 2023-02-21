#include <stdio.h>
#include "binary_tree.h"

node_t *construct_maximum_binary_tree(int *arr, int len)
{
	if(len < 1) return NULL;
	// 获取最大值的index
	int max = *arr;
	int cursor = 0;
	for(int i=0; i<len; i++)
	{
		if(*(arr+i) > max) 
		{
			max = *(arr+i);
			cursor = i;
		}
	}
	node_t *node = create(*(arr+cursor));
	node->left = construct_maximum_binary_tree(arr, cursor);
	node->right = construct_maximum_binary_tree(arr+cursor+1, len-cursor-1);
	return node;
}

int main(void)
{
	int source[] = {3, 2, 1, 6, 0, 5};
	node_t * tree = construct_maximum_binary_tree(source, sizeof(source)/sizeof(int));
	preorder_traversal(tree);
	printf("\n");
	return EXIT_SUCCESS;
}
