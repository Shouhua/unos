/**
 * 给你二叉树的根节点root，请你将它展开成一个单链表：
 * 		1. 展开后的单链表应该同样使用相同的结构，其中right指向链表中的下一个节点，左指针始终为NULL
 * 		2. 展开后单链表应该与二叉树先序遍历顺序相同	
 * 比如：root=[1,2,5,3,4,null,6] -> root=[1,null,2,null,3,null,4,null,5,null,6,null]
*/

/**
 * https://labuladong.gitee.io/algo/di-yi-zhan-da78c/shou-ba-sh-66994/dong-ge-da-cbce8/
*/

#include <stdio.h>
#include "binary_tree.h"

void flat_tree(node_t *root)
{
	if(root == NULL) return;
	flat_tree(root->left);
	flat_tree(root->right);

	node_t *left = root->left;
	node_t *right = root->right;
	root->left = NULL;
	root->right = left;
	node_t *p = root;
	while(p->right != NULL)
	{
		p = p->right;
	}
	p->right = right;
}

int main(void)
{
	node_t *root = create(1);
	insert_left(root, 2);
	insert_right(root, 5);
	insert_left(root->left, 3);
	insert_right(root->left, 4);
	insert_left(root->right, -1);
	insert_right(root->right, 6);

	flat_tree(root);

	preorder_traversal(root);
	printf("\n");
}