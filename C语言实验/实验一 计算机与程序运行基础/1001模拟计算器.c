#include <stdio.h>
#include <ctype.h>

int main() {
    char expression[100];
    int numbers[50];
    char operators[50];
    int num_count = 0;
    int op_count = 0;
    
    // 读取算式
    int i = 0;
    char ch;
    while ((ch = getchar()) != '=') {
        expression[i++] = ch;
    }
    expression[i] = '\0';
    
    // 解析算式
    i = 0;
    while (expression[i] != '\0') {
        // 读取数字
        if (isdigit(expression[i])) {
            int num = 0;
            while (isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            numbers[num_count++] = num;
        }
        
        // 读取运算符
        if (expression[i] == '+' || expression[i] == '-' || 
            expression[i] == '*' || expression[i] == '/') {
            operators[op_count++] = expression[i];
            i++;
        } else if (expression[i] != '\0' && !isdigit(expression[i])) {
            // 非法运算符
            printf("错误的运算符:%c", expression[i]);
            return 0;
        }
    }
    
    // 计算结果（从左到右顺序计算）
    int result = numbers[0];
    for (int j = 0; j < op_count; j++) {
        switch (operators[j]) {
            case '+':
                result += numbers[j + 1];
                break;
            case '-':
                result -= numbers[j + 1];
                break;
            case '*':
                result *= numbers[j + 1];
                break;
            case '/':
                if (numbers[j + 1] == 0) {
                    printf("错误的运算符:/");
                    return 0;
                }
                result /= numbers[j + 1];
                break;
        }
    }
    
    printf("%d", result);
    return 0;
}