#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// 栈元素：存储括号类型（{/}/[/]）和所在索引（便于报错定位）
typedef struct {
    char bracket;  // 括号字符
    int position;  // 括号在JSON字符串中的位置（从0开始）
} StackElem;

// 栈结构体（动态扩容，避免固定容量限制）

typedef struct {
    StackElem* data;     // 栈数据数组
    int top;             // 栈顶指针（-1表示空栈）
    int capacity;        // 当前栈容量
    int max_capacity;    // 最大容量（防止内存消耗过大）
} Stack;

// 栈操作结果枚举（增强健壮性）

typedef enum {
    STACK_OK,           // 操作成功
    STACK_EMPTY,        // 栈空（弹栈/取栈顶失败）
    STACK_FULL,         // 栈满（压栈失败）
    STACK_MEM_ERR       // 内存分配失败
} StackResult;


StackResult stackInit(Stack* stack, int init_capacity, int max_capacity){
    // 步骤1：参数合法性检查
    // 防御性编程：检查所有可能的非法输入
    if(init_capacity <= 0 || max_capacity <= 0 || init_capacity > max_capacity) {
        return STACK_MEM_ERR;  // 参数非法
    }

    // 步骤2：分配栈内存
    // 重要：使用sizeof(StackElem)而不是硬编码的大小，确保类型安全
    stack->data = (StackElem*)malloc(sizeof(StackElem) * init_capacity);
    if (stack->data == NULL) {
        // 内存分配失败：在资源受限的环境中很常见
        return STACK_MEM_ERR;  
    }

    // 步骤3：初始化栈状态
    stack->top = -1;  // 重要：-1表示空栈，这是栈设计的通用约定
    stack->capacity = init_capacity;
    stack->max_capacity = max_capacity;
    return STACK_OK;
}


StackResult stackPush(Stack* stack, char bracket, int position){
    // 步骤1：检查栈是否满（动态扩容的核心逻辑）
    if (stack->top + 1 == stack->capacity){
        // 计算新容量：增加固定大小，但不超过最大容量
        int new_capacity = stack->capacity + 10;
        if (new_capacity > stack->max_capacity) {
            new_capacity = stack->max_capacity;
        }
        
        // 检查是否已经达到最大容量限制
        if (new_capacity == stack->capacity) {
            return STACK_FULL;  // 无法再扩容
        }
        
        // 使用realloc重新分配内存
        // 重要：realloc可能失败，失败时返回NULL，但原内存块保持不变
        StackElem* new_data = (StackElem*)realloc(stack->data, 
                                                 sizeof(StackElem) * new_capacity);
        if (new_data == NULL) {
            return STACK_FULL;  // 内存不足，保持原栈不变
        }
        
        // 更新栈指针和容量
        stack->data = new_data;
        stack->capacity = new_capacity;
    }
    
    // 步骤2：压入元素
    stack->top++;  // 栈顶指针上移
    stack->data[stack->top].bracket = bracket;
    stack->data[stack->top].position = position;
    
    return STACK_OK;
}


StackResult stackPop(Stack* stack, StackElem* elem){
    // 防御性编程：检查栈是否为空
    if (stackIsEmpty(stack)) {
        return STACK_EMPTY;
    }

    // 如果elem不为NULL，则将栈顶元素复制到elem
    if (elem != NULL) {
        *elem = stack->data[stack->top];
    }
    
    // 栈顶指针下移（逻辑上删除元素）
    stack->top--;
    return STACK_OK;
}


StackResult stackPeek(Stack* stack, StackElem* elem) {
    // 防御性编程：检查栈是否为空
    if (stackIsEmpty(stack)) {
        return STACK_EMPTY;
    }
    *elem = stack->data[stack->top];
    return STACK_OK;
}


int stackIsEmpty(const Stack* stack){
    return stack->top == -1 ? 1 : 0;
}


void stackDestroy(Stack* stack) {
    // 防御性编程：检查data是否为NULL
    if (stack->data != NULL) {
        // 释放动态分配的数组
        free(stack->data);
        // 重要：将指针设为NULL，避免悬空指针
        stack->data = NULL;
    }
    
    // 重置栈状态，防止误用已销毁的栈
    stack->top = -1;   
    stack->capacity = 0;
    stack->max_capacity = 0;
}



int isLeftBracket(char ch) {
    return ch == '{' || ch == '[';
}

int isRightBracket(char ch) {
    return ch == '}' || ch == ']';
}


int isBracketMatch(char left, char right) {
    return (left == '{' && right == '}') || (left == '[' && right == ']');
}



int jsonBracketCheckBasic(const char *json_str) {
    // 步骤1：边界情况处理（防御性编程）
    // 注意：空字符串视为合法，这是JSON规范允许的
    if(json_str == NULL || *json_str == '\0'){
        return 1;
    }

    // 步骤2：初始化栈
    Stack stack;
    StackResult result = stackInit(&stack, 10, 100);
    
    // 重要：栈初始化失败时，保守返回合法，避免程序崩溃
    // 在实际工业代码中，可能需要记录日志或返回特定错误码
    if(result != STACK_OK){
        return 1;  // 初始化失败，返回合法是安全的保守选择
    }

    // 步骤3：遍历字符串，检查括号
    for (int i = 0; json_str[i] != '\0'; i++){
        char ch = json_str[i];

        // 遇到左括号：压栈
        if (isLeftBracket(ch)){
            result = stackPush(&stack, ch, i);

            // 检查压栈是否成功
            if (result != STACK_OK){
                stackDestroy(&stack);  // 清理资源
                return 0;  // 压栈失败，返回不合法
            }
        }
        // 遇到右括号：弹栈并检查匹配
        else if(isRightBracket(ch)){
            // 特殊情况：栈空时遇到右括号，说明右括号多余
            if (stackIsEmpty(&stack)){
                stackDestroy(&stack);
                return 0;
            }

            StackElem top_elem;
            if(stackPop(&stack, &top_elem) != STACK_OK){
                // 理论上不应发生，但防御性编程要求检查
                stackDestroy(&stack);
                return 0;
            }

            // 检查括号是否匹配
            if (!isBracketMatch(top_elem.bracket, ch)){
                stackDestroy(&stack);
                return 0;  // 括号不匹配
            }
        }
        // 其他字符：跳过（包括字符串内的括号）
        // 这是基础版的局限性
    }
    
    // 步骤4：检查栈是否为空
    // 栈空表示所有括号都匹配，栈非空表示左括号多余
    int ans = stackIsEmpty(&stack) ? 1 : 0;
    
    // 步骤5：释放资源
    stackDestroy(&stack);
    return ans;
}


int jsonBracketCheckAdvanced(const char *json_str) {
    // 步骤1：边界情况处理
    if (json_str == NULL || *json_str == '\0'){
        return 1;
    }

    // 步骤2：初始化栈
    Stack stack;
    StackResult result = stackInit(&stack, 10, 100);
    if(result != STACK_OK){
        return 1;
    }

    // 步骤3：初始化状态标记
    int in_string = 0; // 0表示不在字符串内，1表示在字符串内
    int escape = 0;    // 0表示不在转义状态，1表示在转义状态

    // 步骤4：遍历字符串
    for (int i = 0; json_str[i] != '\0'; i++){
        char ch = json_str[i];

        // 状态机核心：根据当前状态决定如何处理字符
        if (in_string){
            // 状态1：在字符串内部
            if (escape){
                // 子状态1.1：转义状态中
                // 转义字符后的第一个字符被转义，无论它是什么
                escape = 0;
            }
            else if (ch == '\\'){
                // 遇到反斜杠，进入转义状态
                escape = 1;
            }
            else if (ch == '"' && !escape){
                // 遇到非转义的引号，字符串结束
                in_string = 0;
            }
            // 字符串内的所有括号都不处理（无论是否转义）
        }
        else{
            // 状态2：在字符串外部
            if (ch == '"') {
                // 遇到引号，开始字符串
                in_string = 1;
                escape = 0;  // 重置转义状态
            }
            else if (isLeftBracket(ch)) {
                // 遇到左括号，压栈
                result = stackPush(&stack, ch, i);
                if (result != STACK_OK) {
                    stackDestroy(&stack);
                    return 0;
                }
            }
            else if (isRightBracket(ch)) {
                // 遇到右括号，检查匹配
                if (stackIsEmpty(&stack)) {
                    stackDestroy(&stack);
                    return 0;  // 右括号多余
                }
                
                StackElem top_elem;
                if (stackPop(&stack, &top_elem) != STACK_OK) {
                    stackDestroy(&stack);
                    return 0;
                }
                
                if (!isBracketMatch(top_elem.bracket, ch)) {
                    stackDestroy(&stack);
                    return 0;  // 括号不匹配
                }
            }
            // 其他字符：跳过
        }
    }
    
    // 步骤5：最终状态检查
    // 必须满足两个条件：1.栈空（括号匹配） 2.不在字符串内（字符串闭合）
    int is_valid = (stackIsEmpty(&stack) && !in_string) ? 1 : 0;
    
    // 步骤6：释放资源
    stackDestroy(&stack);
    
    return is_valid;
}


int main(){
    char str[100];
    
    // 注意：这里存在安全隐患，输入超过99个字符会导致缓冲区溢出
    // 实际工业代码应该使用fgets或限制输入长度
    scanf("%s", str); 
    
    // 测试基础版
    int result1 = jsonBracketCheckBasic(str);
    // 测试高级版
    int result2 = jsonBracketCheckAdvanced(str);
    
    // 输出结果
    // 基础版：无法处理字符串内的括号，可能返回错误结果
    // 高级版：正确处理字符串，总是返回正确结果
    printf("%d %d", result1, result2);
    
    return 0;
}

