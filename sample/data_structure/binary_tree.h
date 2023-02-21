#ifndef _BINARY_TREE_
#define _BINARY_TREE_

#include <stdlib.h>

typedef struct connect_node
{
	int value;
	struct connect_node *left;
	struct connect_node *right;
	struct connect_node *next;
} connect_node_t;

typedef struct node 
{
	int value;
	struct node *left;
	struct node *right;
} node_t;

node_t* create(int value)
{
	node_t *new_node = (node_t *)malloc(sizeof(node_t));
	new_node->value = value;
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}

node_t* insert_left(node_t *root, int value)
{
	root->left = create(value);
	return root->left;
}

node_t* insert_right(node_t *root, int value)
{
	root->right = create(value);
	return root->right;
}

// Preorder traversal
void preorder_traversal(node_t *root)
{
	if(root == NULL) return;
	printf("%d ", root->value);
	preorder_traversal(root->left);
	preorder_traversal(root->right);
}

// Inorder traversal
void inorder_traversal(node_t *root)
{
	if(root == NULL) return;
	inorder_traversal(root->left);
	printf("%d ", root->value);
	inorder_traversal(root->right);
}

// Postorder traversal
void postorder_traversal(node_t *root)
{
	if(root == NULL) return;
	postorder_traversal(root->left);
	postorder_traversal(root->right);
	printf("%d ", root->value);
}

connect_node_t* connect_create(int value)
{
	connect_node_t *new_node = (connect_node_t *)malloc(sizeof(connect_node_t));
	new_node->value = value;
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->next = NULL;
	return new_node;
}

connect_node_t* connect_insert_left(connect_node_t *root, int value)
{
	root->left = connect_create(value);
	return root->left;
}

connect_node_t* connect_insert_right(connect_node_t *root, int value)
{
	root->right = connect_create(value);
	return root->right;
}

#endif