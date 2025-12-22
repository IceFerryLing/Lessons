#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 用户节点定义
typedef struct Node {
    char name[50];
    int level; // 1=普通, 2=VIP, 3=SVIP
    struct Node* next;
} Node;

// 队列结构体
typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

// 初始化队列
// 参数：q-队列
void initQueue(Queue* q) {
    // TODO：完成队列初始化代码，队头用front,队尾用rear表示
    q->front = NULL;
    q->rear = NULL;
    
}

// 创建节点
Node* createNode(const char* name, int level) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->level = level;
    newNode->next = NULL;
    return newNode;
}

// Request操作：入队（带优先级插入）
// 参数：q-队列，name-用户名，level-等级
void request(Queue* q, const char* name, int level) {
    Node* newNode = createNode(name, level);
    // 队列为空，直接加入
    if (q->front == NULL) {
        // TODO：完成队列为空的逻辑
        q->front = newNode;
        q->rear = newNode;
        return;
    }

    //寻找插入点
    Node* prev = NULL;
    Node* curr = q->front;

    while (curr != NULL && curr->level >= level){
        prev = curr;
        curr = curr->next;
    }

    if (prev == NULL){
        newNode->next = q->front;
        q->front = newNode;
    }else{
        prev->next = newNode;
        newNode->next = curr;
    }

    //注意队尾处理
    if (curr == NULL){
        q->rear = newNode;
    }
    // 插入逻辑：高等级插在低等级用户前面，同等级则排在后面
    // TODO：请按照该部分的要求补全代码
    
}

// Download操作：输出队首元素，并删除该元素
// 参数：q-队列
void download(Queue* q) {
    if (q->front == NULL) {
        printf("队列为空，没有可下载的任务。\n");
        return;
    }
    Node* temp = q->front;
    printf("下载中：%s (level=%d)\n", temp->name, temp->level);
    //TODO：继续完成删除队首元素的逻辑
    q->front = q->front->next;
    if(q->front == NULL){
        q->rear =NULL;
    }
    free(temp);
}

// Remove操作：删除指定用户
// 参数：q-队列，name-用户名
void removeUser(Queue* q, const char* name) {
    if (q->front == NULL) {
        printf("队列为空，无法删除。\n");
        return;
    }
    Node* prev = NULL;
    Node* curr = q->front;
    while (curr != NULL && strcmp(curr->name, name) != 0) {
        prev = curr;
        curr = curr->next;

    }
    if (curr == NULL) {
        printf("未找到用户：%s\n", name);
        return;
    }
    // 删除指定用户，注意考虑队头和队尾的情况
    // TODO：请完善该部分代码

    if (prev == NULL){
        q->front = curr->next;
    }else{
        prev->next = curr->next;
    }

    if (curr == q->rear){
        q->rear = prev;
    }
    free(curr);
    printf("已删除用户：%s\n", name);
}


// Display操作：显示队列
// 参数：q-队列
void display(Queue* q) {
    if (q->front == NULL) {
        printf("当前队列为空。\n");
        return;
    }
    Node* curr = q->front;
    printf("当前下载队列：\n");
    while (curr != NULL) {
        printf("用户名: %-10s | level: %d\n", curr->name, curr->level);
        curr = curr->next;
    }
}

// 主函数：模拟操作命令
int main() {
    Queue q;
    initQueue(&q);

    char command[50];
    char name[50];
    int level;
    printf("=== 某度网盘下载调度系统 ===\n"); 
    printf("支持命令：Request 用户名 level | Download | Remove 用户名 | Display | Exit\n");
    printf("请输入命令：\n");
    while (1) {
        scanf("%s", command);
        if (strcmp(command, "Request") == 0) {
            scanf("%s %d", name, &level);
            // 完善用户等级判断逻辑，当出现无效用户等级时，输出：“无效的用户等级，请输入 1（普通） 2（VIP） 3（SVIP）。”，然后换行。
            // TODO：
            if(level < 1 || level > 3){
                printf("无效的用户等级，请输入 1（普通） 2（VIP） 3（SVIP）。\n");
                continue;
            }

            request(&q, name, level);
            printf("用户 %s (level=%d) 已加入队列。\n", name, level);

        } else if (strcmp(command, "Download") == 0) {
            download(&q);

        } else if (strcmp(command, "Remove") == 0) {
            scanf("%s", name);
            removeUser(&q, name);

        } else if (strcmp(command, "Display") == 0) {
            display(&q);

        } else if (strcmp(command, "Exit") == 0) {
            printf("程序已退出。\n");
            break;
        } else {
            printf("无效命令，请重新输入。\n");
        }
        // 完善代码逻辑，当输入无效命令时，请输出“无效命令，请重新输入。”，然后换行
        // TODO：
    }

    return 0;
}