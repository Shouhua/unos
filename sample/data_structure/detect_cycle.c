/**
 * 1. 首先找列表的中点，快慢指针，快指针走2步，慢指针走1步
 * 2. detect cycle，一样的原理，如果快慢指针相遇了，说明有环，如果快指针NULL，说明没有环
 * 3. 检查环首次出现的地方，原理相似，只不过根据2，遇到了后，将任意一个指针知道head，然后步长均为一步，下次相遇点就是所求点
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
	cur->next = header->next;
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


node_t * detect_cycle(node_t * head) {
    node_t *fast;
	node_t *slow;
    fast = slow = head;
    while (fast != NULL && fast->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
        if (fast == slow) break;
    }
    // 上面的代码类似 hasCycle 函数
    if (fast == NULL || fast->next == NULL) {
        // fast 遇到空指针说明没有环
        return NULL;
    }

    // 重新指向头结点
    slow = head;
    // 快慢指针同步前进，相交点就是环起点
    while (slow != fast) {
        fast = fast->next;
        slow = slow->next;
    }
    return slow;
}


int main(int argc, char *argv[])
{
	int list_dataset[] = {10, 9, 20, 2, 3, 4};
	node_t *list = initialize_list(list_dataset, sizeof(list_dataset) / sizeof(list_dataset[0]));

	node_t *result = detect_cycle(list);
    if(result == NULL)
    {
        printf("没有环...\n");
    }
	else
	{
		printf("node value: %d\n", result->data);
	}

	free(list);

	return 0;
}