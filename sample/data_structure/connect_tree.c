/**
 * 完美二叉树，就是说整个二叉树是一个正三角形，除了最右侧节点next指向NULL，其他节点右侧都指向相邻节点
 * 遍历
 * 这个连接让人联想起b+树，也是叶子节点连在一起
 * 分解思维/递归 不行 
*/

#include <stdio.h>
#include "binary_tree.h"

void traverse(connect_node_t *left, connect_node_t *right)
{
	if(left == NULL || right == NULL) return;
	left->next = right;

	traverse(left->left, left->right);
	traverse(left->right, right->left);
	traverse(right->left, right->right);
}

connect_node_t * connect_tree(connect_node_t *root)
{
	if(root == NULL) return NULL;
	traverse(root->left, root->right);
	return root;
}

int main(void)
{
	connect_node_t *root = connect_create(4);
	connect_insert_left(root, 2);
	connect_insert_right(root, 7);
	connect_insert_left(root->left, 1);
	connect_insert_right(root->left, 3);
	connect_insert_left(root->right, 6);
	connect_insert_right(root->right, 9);

	connect_tree(root);
	// GDB check root data
	return EXIT_SUCCESS;
}