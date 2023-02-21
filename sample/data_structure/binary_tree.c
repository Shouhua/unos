/**
 * https://labuladong.gitee.io/algo/di-yi-zhan-da78c/shou-ba-sh-66994/dong-ge-da-334dd/
 * 二叉树(binary tree)，展示二叉树的前中后序
 * 前中后序是遍历二叉树过程中处理每一个节点的三个特殊时间点，绝不仅仅是三个顺序不同的 List：
 * 		前序位置的代码在刚刚进入一个二叉树节点的时候执行；
 * 		后序位置的代码在将要离开一个二叉树节点的时候执行；
 * 		中序位置的代码在一个二叉树节点左子树都遍历完，即将开始遍历右子树的时候执行。
二叉树解题的思维模式分两类：
	1、是否可以通过遍历一遍二叉树得到答案？如果可以，用一个 traverse 函数配合外部变量来实现，这叫「遍历」的思维模式。
	2、是否可以定义一个递归函数，通过子问题（子树）的答案推导出原问题的答案？如果可以，写出这个递归函数的定义，并充分利用这个函数的返回值，这叫「分解问题」的思维模式。
无论使用哪种思维模式，你都需要思考：
如果单独抽出一个二叉树节点，它需要做什么事情？需要在什么时候（前/中/后序位置）做？其他的节点不用你操心，递归函数会帮你在所有节点上执行相同的操作。 
*/
#include <stdio.h>
#include "binary_tree.h"

int main(void)
{
	node_t *root = create(1);
	insert_left(root, 2);
	insert_right(root, 3);
	insert_left(root->left, 4);
	insert_right(root->left, 5);
	insert_left(root->right, 6);
	insert_right(root->right, 7);

	printf("Traversal of the inserted binary tree \n");
	printf("Inorder traversal \n");
	inorder_traversal(root);
	printf("\n");

	printf("Preorder traversal \n");
	preorder_traversal(root);
	printf("\n");

	printf("Postorder traversal \n");
	postorder_traversal(root);
	printf("\n");

	return EXIT_SUCCESS;
}
