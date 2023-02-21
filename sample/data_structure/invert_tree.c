/**
 * 二叉树反转
*/

#include <stdio.h>
#include "binary_tree.h"

/* 方法一，使用前或者后序
// void traverse(node_t *root)
// {
// 	if(root == NULL) return;
// 	node_t *tmp = root->left;
// 	root->left = root->right;
// 	root->right = tmp;

// 	traverse(root->left);
// 	traverse(root->right);
// }

// node_t *invert_tree(node_t *root)
// {
// 	traverse(root);
// 	return root;
// }
*/

// 方法二 使用分解/递归方式解决
node_t * invert_tree(node_t *root)
{
	if(root == NULL) return NULL;
	node_t *left = invert_tree(root->left);
	node_t *right = invert_tree(root->right);

	root->left = right;
	root->right = left;
	return root;
}

int main(void)
{
	node_t *root = create(4);
	insert_left(root, 2);
	insert_right(root, 7);
	insert_left(root->left, 1);
	insert_right(root->left, 3);
	insert_left(root->right, 6);
	insert_right(root->right, 9);

	invert_tree(root);

	preorder_traversal(root);
	printf("\n");
	// inorder_traversal(root);
	// printf("\n");
	// postorder_traversal(root);
	// printf("\n");

	return EXIT_SUCCESS;
}