/**
 * 一直2个list，判断是否存在交叉重合的节点，如果有，节点是哪个
 *       a1 -> a2
 *              |
 *               -> c1 -> c2
 *              |
 * b1 -> b2 -> b3
 * 比如上图中就是有重合节点c1
 * 解决方案1：2个指针，分别从list1和list2跑，跑完了就分别继续跑list2和list1， 如果有重合的，肯定在跑完前相等
 * 解决方案2：跟检查环差不多，可以将list1和list2收尾相连，就变成了前面的在一条list中寻找环起点的问题，但是这个改变了原始list的结构
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data;
	struct node *next;
} node_t;

node_t* initialize_list(int *dataset, int count, node_t *node)
{
    node_t *header = (node_t *)malloc(sizeof(node_t));
    node_t *cur = header;
    cur->data = dataset[0];
	for(int i = 1; i < count; i++)
    {
        cur->next = (node_t *)malloc(sizeof(node_t));         
        cur->next->data = dataset[i];
        cur = cur->next;
    }
    cur->next = node;
    return header;
}

void print_list(node_t *list)
{
    while(list != NULL)
    {
        printf("%d\t", list->data);
        list = list->next;
    }
    printf("\n");
}


node_t * get_intersection_node(node_t *list1, node_t *list2) 
{
    node_t *cur1 = list1;
    node_t *cur2 = list2;

    while(cur1 != NULL || cur2 != NULL)
    {
        if(cur1 == NULL)
        {
            cur1 = list2;
        }
        if(cur2 == NULL)
        {
            cur2 = list1;
        }
        if(cur1 == cur2)
        {
            return cur1;
        }
        cur1 = cur1->next;
        cur2 = cur2->next;
    }
}


int main(int argc, char *argv[])
{
	int list_dataset1[] = {10, 9, 20};
	int list_dataset2[] = {1, 2};

    node_t *c2 = (node_t *)malloc(sizeof(node_t));
    c2->data = 10000;
    node_t *c1 = (node_t *)malloc(sizeof(node_t));
    c1->data = 20000;
    c1->next = c2;

	node_t *list1 = initialize_list(list_dataset1, sizeof(list_dataset1) / sizeof(list_dataset1[0]), c1);
	node_t *list2 = initialize_list(list_dataset2, sizeof(list_dataset2) / sizeof(list_dataset2[0]), c1);

	node_t *result = get_intersection_node(list1, list2);
    if(result == NULL)
    {
        printf("没有重合节点...\n");
    }
	else
	{
		printf("node value: %d\n", result->data);
	}

	free(list1);
	free(list2);

	return 0;
}