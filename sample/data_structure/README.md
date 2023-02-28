https://labuladong.gitee.io/algo/di-ling-zh-bfe1b/wo-de-shua-5fe0c/
## 问题
- 合并两个有序链表
- 单链表的分解
- 合并k个有序链表
- 单链表的倒数第k个节点
- 单链表的中电
- 判断链表中是否有环
- 两个链表是否相交

### 链表
- 不知道长度

### 数组
- 知道长度

## b plus tree伪代码
```c
typedef struct record 
{
	int value;
} record;
typedef struct node 
{
	bool is_leaf;
	int *keys;
	int key_num;
	void **pointers;
	node *parent;
} node;
// insert
```
node *make_node()
	node = malloc(sizeof(node))
	node->is_leaf = false
	node->keys = malloc(sizeof(int) * (order - 1));
	node->key_num = 0;
	node->parent = NULL;
	node->pointers = malloc(sizeof(void *) * order);
	return node;
node *make_leaf()
	leaf = make_node()
	leaf.is_leaf = true
	return leaf
node *find_leaf(root, key)
	node *node = root;
	i = 0
	while(!node->is_leaf)
		while(node->keys[i]<=key)
			i++
		end while
		node = node->pointers[i]
	end while
	return node
	
record * find(root, key, node* *leaf_out)
	找到key所在的leaf node, node *find_leaf(root, key)
	leaf_out = leaf_node
	foreach(leaf_node keys)
		if leaf_node->keys[i] == key
			return node->pointers[i]
	end foreach
	return NULL;
node *start_new_tree(key, record)
	root = make_leaf()
	root->keys[0] = key
	root->pointers[0] = record
	root->pointers[order-1] = NULL
	root->parent = NULL
	root->key_num++
	return root
node *insert_into_node(node *root, node *parent, node *right, int left_index, int key)
	foreach[parent->key_num, left_index)
		parent->keys[i] = parent->keys[i-1]
		parent->pointers[i+1] = parent->pointers[i]
	parent->key_num++
	parent->keys[left_index] = key
	parent->pointer[left_index+1] = right
	return root
node *insert_into_node_after_splitting(node *root, node *parent(old_node), node *right, int left_index, int key)
	// temp_keys, temp_pointers	
	node *new_node = make_node()
	int *temp_keys = malloc(order*sizeof(int))
	node **temp_pointers = malloc((order+1)*sizeof(node *))

	foreach[0, number_keys)
		if j == left_index
			j++
		temp_keys[j] = parent->keys[i]
	foreach[0, number_keys+1)
		if j == (left_index + 1)
			j++
		temp_pointers[j] = parent->pointers[i]
	
	temp_keys[left_index] = key
	temp_pointers[left_index+1] = right

	split_index = cut(order)
	parent->key_num = 0
	foreach[0, split_index)
		parent->key_num++
		parent->key[i] = temp_key[i]
		parent->pointers[i] = temp_pointers[i]
	key_prime = temp_keys[split_index]
	for ++i, j=0;i<order;i++,j++
		new_node->keys[j] = temp_keys[i]
		new_node->key_num++
		new_node->pointers[j] = temp_pointers[i]
	new_node->pointers[j] = temp_pointers[i] // pointers比keys多一个
	free(temp_keys)
	free(temp_pointers)
	new_node->parent = parent->parent

	// 更新子节点的parent
	for[0, key_num)
		child = new_node->pointers[k]
		child->parent = new_node

	return insert_into_parent(root, parent, key_prime, new_node)

int get_left_index(node *parent, node *left, int key)
	while(left_index <= parent->key_num && parent->pointers[left_index] !=left)
		left_index++
	return left_index
node *inset_into_parent(node *root, node *left, int key, node *right)
	node *parent = left->parent
	left_index = get_left_index(parent, node *left, int key)
	if(parent->key_num < order-1)
		return insert_into_node(root, parent, right, left_index, key) 
	return insert_into_node_after_splitting()

node *insert_into_leaf(node *leaf_node, int key, record *record)
	insertion_point = 0
	while(insert_point < order -1 && leaf_node->keys[insertion_point]<=key)
		insertion_point++
	for(i=leaf_node->key_num, i>insertion_point, i--)
		leaf_node->keys[i] = leaf_node->keys[i-1]
		leaf_node->pointers[i] = leaf_node->pointers[i-1]
	leaf_node->pointers[insertion_point] = record
	leaf_node->key[insertion_point] = key
	return leaf_node
int cut(int order)
	return order/2;
node *insert_into_leaf_after_splitting(node *root, node *leaf_node, int key, record *record)
	// 使用temp_keys, temp_pointers装下插入后的
	int* temp_keys = malloc(sizeof(order * sizeof(int)))
	void **temp_keys = malloc(sizeof(order * sizeof(void *)))
	while(insert_point < order -1 && leaf_node->keys[insertion_point]<=key)
		insertion_point++
	foreach[0, leaf_node->key_num)
		if i == insert_point
			j++
		temp_keys[j] = leaf_node[i]
		temp_pointers[j] = leaf_node[i]
	// 优化前代码
	foreach[0, insertion_point)
		temp_keys[i] = leaf_node[i]
		temp_pointers[i] = leaf_node[i]
	foreach(leaf_node->key_num, insertion_point)
		temp_keys[j++] = leaf_node->keys[j]
		temp_pointers[j++] = leaf_node[j]
	temp_keys[insertion_point] = key
	temp_pointers[insertion_point] = record
	// 然后根据cut点分割成leaf_node, new_node
	split_index = cut(order)
	// 重新复制leaf_node的keys, pointers
	leaf_node->key_num = 0
	foreach[0, split_index)
		leaf_node->keys[i] = temp_keys[i]
		leaf_node->key_num++
		leaf_node->pointers[i] = temp_pointers[i]
	for i=split,j=0;i<order;i++,j++
		new_leaf->keys[j] = temp_keys[i]
		new_leaf->key_num++
		new_leaf->pointers[j] = temp_pointers[i]
	new_leaf->pointers[order-1] = leaf_node->pointers[order-1]
	new_leaf->parent = leaf_node->parent
	leaf_node->pointers[order-1] = new_leaf
	free(temp_keys)
	free(temp_pointers)
	// 设置leaf_node, new_leaf的余下pointers
	foreach[key_num, order-1)
		leaf_node->pointers[i] = NULL
		new_leaf->pointers[i] = NULL
	// 找到新的提交到父节点的key
	new_key = new_leaf->keys[0]
	return insert_into_parent(root, leaf_node, new_key, new_leaf)
node *insert(node *root, int key, int value)
	check if root == NULL, 如果root为NULL，start_new_tree(),  返回root
	// 检查key是否在leaf_node中存在
	if find(root, key, value) is not NULL
		record->value = value
		return root
	// 如果不存在，做插入操作
	if leaf_node->key_num < order-1
		insert_into_leaf(leaf_node, key, value)
		return root
	else
		return insert_into_leaf_after_splitting(root, leaf_node, key, value)

