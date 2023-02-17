#include <stdio.h>
#include <stdlib.h>

// 不同的删除
int removeElement(int nums[], int count, int val)
{
	int index = 0;
	for(int i=0; i<count; i++)
	{
		if(nums[i] != val)
		{
			nums[index] = nums[i];
			index++;
		}
	}
	return index;
}

int removeDuplicates(int nums[], int count)
{
	if(count < 2) return count;
	int index = 1;
	for(int i = 1; i < count; i++)
	{
		if(nums[i-1] != nums[i])
		{
			if(nums[index] != nums[i])
			{
				nums[index] = nums[i];
			}
			index++;
		}
	}
	return index;
}

int main(void)
{
	int nums[] = { 0, 0, 1, 1, 2, 2, 3, 3, 4 };
	int count = removeDuplicates(nums, 9);
	printf("count: %d\n", count);
	printf("array: ");
	for(int i=0;i<9;i++)
	{
		printf("%d\t", nums[i]);
	}
	printf("\n");

	int nums1[] = { 0, 1, 2,2,3,0,4,2};
	count = removeElement(nums1, 8, 2);
	printf("count: %d\n", count);
	printf("array: ");
	for(int i=0;i<count;i++)
	{
		printf("%d\t", nums1[i]);
	}
	printf("\n");

	return EXIT_SUCCESS;
}