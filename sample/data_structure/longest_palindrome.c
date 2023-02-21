/**
 * 找到字符串中最长的回文
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct my_str {
    char *str;
    int count;
} my_str_t;

my_str_t find_palindrome(char *str, int l, int r, int count)
{
    int index;
    while(l >= 0 && r <=count && str[l] == str[r])
    {
        l--;
        r++;
        index++;
    }
    char *result = (char *)malloc(sizeof(r - l - 1));
    memcpy(result, str+l+1, r - l - 1);
    my_str_t result_str = {
        result,
        r-l-1
    };
    // printf("find: %d, %d, %s, %d\n", l, r, result_str.str, result_str.count);
    return result_str;
}

my_str_t longest_palindrome(char * str, int count)
{
    my_str_t result = { NULL, 0};
    for(int i=0; i<count; i++)
    {
        my_str_t i1 = find_palindrome(str, i, i, count);         
        my_str_t i2 = find_palindrome(str, i, i+1, count);         
        // printf("%s, %d, %d, %s\n", i1.str, i1.count, result.count, result.str);
        result = result.count > i1.count ? result : i1;
        result = result.count > i2.count ? result : i2;
    }
    return result; 
}

int main(void)
{
    char str[] = {'b', 'a', 'b', 'a', 'b', 'd'};
    my_str_t result = longest_palindrome(str, sizeof(str)/sizeof(char));
    printf("最长回文为: %s\n", result.str);
}