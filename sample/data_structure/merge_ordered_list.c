/**
 * 合并两个有序列表, 注意结果列表的虚拟头
*/
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data;
	struct node *next;
} node_t;

node_t* initialize_list(int *dataset, int count)
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
    return header;
}

node_t * merge_ordered_list(node_t *list1, node_t *list2)
{
	node_t *result = (node_t *)malloc(sizeof(node_t));
    result->data = -1;
    node_t *res_cur = result;
    while(list1!=NULL && list2!=NULL)
    {
        if(list1->data < list2->data)
        {
            res_cur->next = list1;
            list1 = list1->next;
        }
        else
        {
            res_cur->next = list2;
            list2 = list2->next;
        }
        res_cur = res_cur->next;
    }
    if(list1 != NULL)
    {
        res_cur->next = list1;
    }
    if(list2 != NULL)
    {
        res_cur->next = list2;
    }
    return result;
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

int main(int argc, char *argv[])
{
	int list1_dataset[] = {2, 3, 4};
	int list2_dataset[] = {1, 3, 10};

	node_t *list1 = initialize_list(list1_dataset, sizeof(list1_dataset) / sizeof(list1_dataset[0]));
    printf("List1: \n");
    print_list(list1);
	node_t *list2 = initialize_list(list2_dataset, sizeof(list2_dataset) / sizeof(list2_dataset[0]));
    printf("List2: \n");
    print_list(list2);

    node_t *result = merge_ordered_list(list1, list2);
    printf("Merged list: \n");
    print_list(result->next);

    free(list1);
    free(list2);
    free(result);

	return 0;
}