#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void reverseString(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

int main() {
    char num1[51], num2[51];
    char result[51]; // 加法的结果最多比最长数字多一位
    
    // 读取两个大数
    scanf("%s", num1);
    scanf("%s", num2);
    
    // 反转字符串，从个位开始计算
    reverseString(num1);
    reverseString(num2);
    
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int maxLen = len1 > len2 ? len1 : len2;
    
    int carry = 0; // 进位
    int index = 0;
    
    // 逐位相加
    for (int i = 0; i < maxLen; i++) {
        int digit1 = (i < len1) ? num1[i] - '0' : 0;
        int digit2 = (i < len2) ? num2[i] - '0' : 0;
        int sum = digit1 + digit2 + carry;
        
        result[index++] = (sum % 10) + '0'; // 当前位
        carry = sum / 10; // 进位
    }
    
    // 处理最后的进位
    if (carry > 0) {
        result[index++] = carry + '0';
    }
    
    // 结束字符串
    result[index] = '\0';
    
    // 反转结果，得到正确顺序
    reverseString(result);
    
    // 输出结果
    printf("%s\n", result);
    
    return 0;
}