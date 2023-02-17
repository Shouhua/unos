/**
 * 给定一个列表，要求得到倒数第N个节点
 * 删除列表中倒数第N个节点原理一样
 * 比如给定[1, 2, 10, 8, 9]，要求获取倒数第2个节点，难点是节点是单向的，而且长度是未知的
 * 使用比如长度为L，只要头指针走L-N+1，但是L无法获得，那可以有两个指针相隔N，待后面指针到头后，前面指针位置就是所求点
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

void print_list(node_t *list)
{
    while(list != NULL)
    {
        printf("%d\t", list->data);
        list = list->next;
    }
    printf("\n");
}

node_t * find_from_end(node_t *list, int count)
{
    node_t *cur = list;
	node_t *head = NULL;
    node_t *tail = NULL;

    int cursor = 0;

    while(cur != NULL)
    {
        cursor++;
        if(cursor == count)
        {
            head = list;
            tail = cur;
        }
        else if(cursor > count)
        {
            head = head->next;
            tail = tail->next;
        }
        else
        {
            // do nothing
        }
        cur = cur->next;
    }
    if(cursor < count)
    {
        return NULL;
    }
    return head;
}

int main(int argc, char *argv[])
{
	int list_dataset[] = {10, 9, 20, 2, 3, 4};
	node_t *list = initialize_list(list_dataset, sizeof(list_dataset) / sizeof(list_dataset[0]));

	node_t *result = find_from_end(list, 6);
    if(result == NULL)
    {
        printf("是不是参数给的太大了，超过列表长度了...\n");
        return -1;
    }
	printf("node value: %d\n", result->data);

	free(list);

	return 0;
}