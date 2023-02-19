/*
判断字符串是否为回文
*/
#include <stdio.h>

// 0 不是，1 是
int is_palindrome(char *str, int count)
{
    for(int i = 0; i < count/2; i++)
    {
        if(str[i] != str[count-i-1])
        {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    char str[] = {'a', 'b', 'b', 'c', 'a'};
    int result = is_palindrome(str, sizeof(str)/sizeof(char));
    printf("%s%s回文", str, result > 0 ? "是" : "不是");
    return 0;
}