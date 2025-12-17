#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>    // Sleep()
#else
#include <unistd.h>     // sleep()
#endif

#define MAX_RECORDS 100
#define SCORE_FILE  "score.txt"

/* 一个成绩记录 */
typedef struct
{
    char name[20];      // 玩家姓名（无空格）
    int  ms;            // 反应时间（毫秒）
    char date[11];      // 日期：YYYY-MM-DD 字符串（10+1）
    char time_str[9];   // 时间：HH:MM:SS 字符串（8+1）
} Record;

/* 函数声明 */
void clear_input_buffer(void);
int  load_records(Record rec[], int max_count);
void show_records(const Record rec[], int count);
int  play_game(void);
int add_record(Record rec[], int *count, int ms);
void wait_random_seconds(int min_s, int max_s);

int main(void)
{
    Record records[MAX_RECORDS];
    int count;
    int choice;

    // 初始化随机种子，用于随机等待时间
    srand((unsigned int)time(NULL));

    // 1. 从 score.txt 加载历史成绩记录
    count = load_records(records, MAX_RECORDS);
    printf("已从 %s 加载 %d 条记录。\n\n", SCORE_FILE, count);

    // 2. 菜单循环
    while (1)
    {
        printf("---------- 反应时间测试 ----------\n");
        printf("1. 开始游戏\n");
        printf("2. 显示记录\n");
        printf("0. 退出\n");
        printf("请选择: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("无效输入，请重试。\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();   // 清除数字后面的换行符

        if (choice == 0)
        {
            printf("再见！\n");
            break;
        }

        if (choice == 1)
        {
            // 3. 玩一次游戏，获得反应时间 T（毫秒）
            int ms = play_game();
            if (ms < 0)
            {
                printf("游戏失败，请重试。\n");
                continue;
            }

            printf("保存此记录？(y/n): ");
            int ch = getchar();
            clear_input_buffer();

            if (ch == 'y' || ch == 'Y')
            {
                // 4. 将记录追加到内存数组和文件
                if (add_record(records, &count, ms) == 1)
                    printf("记录已保存。\n");
                else
                    printf("保存记录失败。\n");
            }
        }
        else if (choice == 2)
        {
            // 5. 查看成绩列表
            show_records(records, count);
        }
        else
        {
            printf("无效选择，请重试。\n");
        }

        printf("\n");
    }

    return 0;
}

/* 清空输入缓冲区，确保后续 getchar 不会读到之前的回车键 */
void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        /* 丢弃所有字符 */
    }
}

/* 随机等待 min_s ~ max_s 秒 */
void wait_random_seconds(int min_s, int max_s)
{
    int delay = rand() % (max_s - min_s + 1) + min_s;
#ifdef _WIN32
    Sleep(delay * 1000);
#else
    sleep(delay);
#endif
}

/* 一次游戏：随机等待 + 计时
   返回反应时间（毫秒），超时返回 -1 */
int play_game(void)
{
    int ch;
    clock_t start, end;
    double elapsed_ms;

    printf("\n(随机等待中，请不要按任何键...)\n");
    wait_random_seconds(1, 3);

    printf("现在按 Enter 键！\n");
    fflush(stdout);   // 确保提示信息显示出来

    // 开始计时
    start = clock();

    // 等待玩家按下 Enter 键，即 '\n' 字符
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        // 什么也不做，只是等待回车
    }

    end = clock();

    elapsed_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("你的反应时间: %.0f 毫秒。\n", elapsed_ms);

    if (elapsed_ms < 0)
    {
        return -1;
    }
    return (int)(elapsed_ms + 0.5);  // 四舍五入
}

/* =========================================================
   TODO-1: load_records
   功能：
     从成绩文件 SCORE_FILE（文本格式）读取历史记录，
     填充到记录数组 rec 中，返回读取的数量。

   参数：
     rec       - 记录数组（输出参数）
     max_count - 数组的最大容量。

   返回值：
     成功读取的记录数，0 表示没有记录或文件不存在。

   文件格式：每行：
     name ms YYYY-MM-DD HH:MM:SS
     例如：Alice 320 2025-11-29 14:03:59
   ========================================================= */
int load_records(Record rec[], int max_count)
{
    FILE *fp = fopen("score.txt", "r");  // 以只读方式打开文件
    int count = 0;      // 已加载记录数
    Record Student;      // 临时存储一条记录

    if (fp == NULL) {   // 文件不存在或无法打开
        return 0;
    }

    // 循环读取文件内容
    while (fscanf(fp, "%s %d %s %s", Student.name, &Student.ms, Student.date, Student.time_str) == 4) {
        if (count < max_count) {  // 确保不超过数组容量
            rec[count++] = Student;  // 添加到数组
        } else {
            break;  // 达到最大容量则停止读取
        }
    }

    fclose(fp);  // 关闭文件
    return count;  // 返回加载的记录数
}

/* =========================================================
   TODO-2: show_records
   功能：
     以表格形式在屏幕上显示所有成绩记录。
     每行显示序号、姓名、反应时间（毫秒）、记录时间。

   参数：
     rec   - 记录数组；
     count - 有效记录数。

   返回值：
     无（void）。
   ========================================================= */
void show_records(const Record rec[], int count)
{
    printf("\n序号 姓名  反应时间(毫秒) 记录时间\n");
    printf("-------------------------------------\n");

    // 遍历数组并打印每条记录
    for (int i = 0; i < count; i++) {
        printf("%d    %-6s %d           %s %s\n",
            i + 1, rec[i].name, rec[i].ms, rec[i].date, rec[i].time_str);
    }
}

/* =========================================================
   add_record
   功能：
     在数组末尾追加一条新记录，并写入 score.txt。

   参数：
     rec[]   - 记录数组（输入/输出）
     count   - 当前记录数指针
     ms      - 本次反应时间（毫秒）

   返回值：
     1   表示成功追加1条记录
    -1   表示出现异常（如数组已满或文件写入失败）
   ========================================================= */
int add_record(Record rec[], int *count, int ms)
{
    // 检查是否达到最大记录数
    if (*count >= MAX_RECORDS) {
        printf("已达到最大记录数限制！\n");
        return -1;
    }

    Record Student;  // 新记录
    printf("请输入姓名:");
    scanf("%19s", Student.name);  // 限制输入长度防止溢出
    Student.ms = ms;  // 设置反应时间

    // 获取当前系统时间
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // 格式化日期和时间字符串
    snprintf(Student.date, 11, "%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    snprintf(Student.time_str, 9, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

    rec[(*count)++] = Student;  // 添加到数组并增加计数

    // 将新记录追加保存到文件
    FILE *fp = fopen("score.txt", "a");
    if (fp == NULL) {
        printf("打开文件失败！\n");
        return -1;
    } else {
        // 写入文件，字段间用空格分隔
        fprintf(fp, "%s %d %s %s\n", Student.name, Student.ms, Student.date, Student.time_str);
        fclose(fp);
        printf("保存成功！\n");
        return 1;  // 成功返回1
    }
}