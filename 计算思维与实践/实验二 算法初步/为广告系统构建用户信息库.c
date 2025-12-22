#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================== 公共类型 & 宏 ================== */

/* 实验里常见的统计结构 */
typedef struct {
    long long compares;
    long long moves;
} Metrics;

#define INTEREST_NOT_FOUND (-1.0f)

/* 客户结构体：包含客户ID和兴趣分值（核心排序依据） */
typedef struct {
    int id;             // 客户唯一标识
    float interest;     // 兴趣分值
} Customer;

/* ================== 参数区 ================== */
#ifndef N_BENCH
#define N_BENCH 20000          /* 数据规模：默认 2e4 */
#endif
#define CAPACITY (N_BENCH + 1024)

/* =============== 工具 & 打印 =============== */
static void print_head_no_count(const char *title, const Customer *arr, int n) {
    printf("---- %s ----\n", title);
    for (int i = 0; i < n; ++i) {
        printf("[%05d] id=%d interest=%.2f\n", i, arr[i].id, arr[i].interest);
    }
}
/* ============ 无序数组 ============ */

/* 线性查找（返回interest或INTEREST_NOT_FOUND） */
// 参数: customers-客户数组, n-数组长度, id-目标id, m-统计指针
float uaFindInterestById(const Customer customers[], int n, int id, Metrics *m) {
    // 1) 每比较一次可执行 if(m) m->compares++;
    // 2) 若 customers[i].id == id 返回 customers[i].interest
    // 3) 未找到返回 INTEREST_NOT_FOUND
    for (int i = 0; i < n; ++i) {
        if (m) ++m->compares;
        if (customers[i].id == id)
            return customers[i].interest;
    }
    return INTEREST_NOT_FOUND;
}

/* 尾部插入（成功返回新n，失败返回-1） */
// 参数: customers-客户数组, n-当前元素个数, capacity-最大容量, c-新客户, m-统计指针
int uaInsertBack(Customer customers[], int n, int capacity, Customer c, Metrics *m) {
    // 1) 若 n == capacity → 返回 -1（容量不足）
    // 2) customers[n] = c; 若统计则 if(m) m->moves++;
    // 3) 返回 n+1
    if (n == capacity)
        return -1;

    customers[n] = c;
    if (m) m->moves++;

    return n + 1;
}

/* 按id删除（成功返回新n，未找到返回-1） */
// 参数: customers-客户数组, n-当前元素个数, id-目标id, m-统计指针
int uaDeleteById(Customer customers[], int n, int id, Metrics *m) {
    // 1) 线性查找目标下标 idx（比较时可 if(m) m->compares++）
    // 2) 未找到 → 返回 -1
    // 3) for i=idx+1..n-1: customers[i-1] = customers[i]; 若统计则 moves++
    // 4) 返回 n-1
    int idx = 0;
    while (idx < n && customers[idx].id != id)
        ++idx;

    if (idx == n)
        return -1;

    for (int i = idx + 1; i < n; ++i) {
        customers[i - 1] = customers[i];
        if (m) m->moves++;
    }

    return n - 1;
}

/* ============ 有序数组（按id升序） ============ */

/* 二分查找（返回interest或INTEREST_NOT_FOUND） */
// 参数: customers-有序数组, n-数组长度, id-目标id, m-统计指针
float oaFindInterestById(const Customer customers[], int n, int id, Metrics *m) {
    // 1) lo=0, hi=n-1
    // 2) while (lo<=hi): mid=(lo+hi)/2
    //    比较时可 if(m) m->compares++;
    //    a) customers[mid].id == id → 返回 customers[mid].interest
    //    b) customers[mid].id < id → lo=mid+1; 否则 hi=mid-1
    // 3) 未找到 → 返回 INTEREST_NOT_FOUND
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (m) m->compares++;

        if (customers[mid].id == id)
            return customers[mid].interest;

        if (customers[mid].id < id)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return INTEREST_NOT_FOUND;
}

/* 保序插入（成功返回新n，失败返回-1） */
// 参数: customers-有序数组, n-当前元素个数, capacity-最大容量, c-新客户, m-统计指针
int oaInsertKeepOrder(Customer customers[], int n, int capacity, Customer c, Metrics *m) {
    // 1) 容量判定: 若 n == capacity → 返回 -1
    // 2) 用二分“下界”定位插入位置 pos: 第一个使 customers[pos].id >= c.id 的位置 (0..n)
    //    比较过程中可 if(m) m->compares++;
    // 3) for i=n..pos+1 递减: customers[i] = customers[i-1]; 若统计 moves++
    // 4) customers[pos] = c; 若统计 moves++
    // 5) 返回 n+1
    if (n == capacity)
        return -1;

    int lo = 0, hi = n - 1, pos = n;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (m) m->compares++;

        if (customers[mid].id < c.id)
            lo = mid + 1;
        else {
            pos = mid;
            hi = mid - 1;
        }
    }

    for (int i = n; i > pos; --i) {
        customers[i] = customers[i - 1];
        if (m) m->moves++;
    }

    customers[pos] = c;
    if (m) m->moves++;

    return n + 1;
}

/* 按id删除（成功返回新n，未找到返回-1） */
// 参数: customers-有序数组, n-当前元素个数, id-目标id, m-统计指针
int oaDeleteById(Customer customers[], int n, int id, Metrics *m) {
    // 1) 先二分查找 idx; 比较时可 if(m) m->compares++;
    // 2) 未找到 → 返回 -1
    // 3) for i=idx+1..n-1: customers[i-1] = customers[i]; 若统计 moves++
    // 4) 返回 n-1
    int lo = 0, hi = n - 1, idx = n;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (m) m->compares++;

        if (customers[mid].id == id) {
            idx = mid;
            break;
        }
        if (customers[mid].id < id)
            lo = mid + 1;
        else
            hi = mid - 1;
    }

    if (idx == n)
        return -1;

    for (int i = idx + 1; i < n; ++i) {
        customers[i - 1] = customers[i];
        if (m) m->moves++;
    }

    return n - 1;
}

/* ================== 主程序：命令驱动 ================== */

int main(void) {
    /* 两个独立的数组：一个无序，一个有序 */
    static Customer ua[CAPACITY];
    static Customer oa[CAPACITY];
    int ua_n = 0;
    int oa_n = 0;

    char cmd[32];

    /* 从标准输入读命令，直到 EOF 或 END */
    while (scanf("%31s", cmd) == 1) {
        if (strcmp(cmd, "UA_INSERT") == 0) {
            int id;
            float interest;
            if (scanf("%d %f", &id, &interest) != 2) {
                /* 输入不合法，直接退出 */
                return 0;
            }
            Customer c = { id, interest };
            int new_n = uaInsertBack(ua, ua_n, CAPACITY, c, NULL);
            if (new_n == -1) {
                printf("UA_INSERT FAIL\n");
            } else {
                ua_n = new_n;
                printf("UA_INSERT OK %d\n", ua_n);
            }
        }
        else if (strcmp(cmd, "UA_FIND") == 0) {
            int id;
            if (scanf("%d", &id) != 1) return 0;
            float v = uaFindInterestById(ua, ua_n, id, NULL);
            if (v == INTEREST_NOT_FOUND) {
                printf("UA_FIND NOT_FOUND\n");
            } else {
                printf("UA_FIND FOUND %.2f\n", v);
            }
        }
        else if (strcmp(cmd, "UA_DELETE") == 0) {
            int id;
            if (scanf("%d", &id) != 1) return 0;
            int new_n = uaDeleteById(ua, ua_n, id, NULL);
            if (new_n == -1) {
                printf("UA_DELETE NOT_FOUND\n");
            } else {
                ua_n = new_n;
                printf("UA_DELETE OK %d\n", ua_n);
            }
        }
        else if (strcmp(cmd, "OA_INSERT") == 0) {
            int id;
            float interest;
            if (scanf("%d %f", &id, &interest) != 2) {
                return 0;
            }
            Customer c = { id, interest };
            int new_n = oaInsertKeepOrder(oa, oa_n, CAPACITY, c, NULL);
            if (new_n == -1) {
                printf("OA_INSERT FAIL\n");
            } else {
                oa_n = new_n;
                printf("OA_INSERT OK %d\n", oa_n);
            }
        }
        else if (strcmp(cmd, "OA_FIND") == 0) {
            int id;
            if (scanf("%d", &id) != 1) return 0;
            float v = oaFindInterestById(oa, oa_n, id, NULL);
            if (v == INTEREST_NOT_FOUND) {
                printf("OA_FIND NOT_FOUND\n");
            } else {
                printf("OA_FIND FOUND %.2f\n", v);
            }
        }
        else if (strcmp(cmd, "OA_DELETE") == 0) {
            int id;
            if (scanf("%d", &id) != 1) return 0;
            int new_n = oaDeleteById(oa, oa_n, id, NULL);
            if (new_n == -1) {
                printf("OA_DELETE NOT_FOUND\n");
            } else {
                oa_n = new_n;
                printf("OA_DELETE OK %d\n", oa_n);
            }
        }
        else if (strcmp(cmd, "PRINT_UA") == 0) {
            print_head_no_count("UNORDERED ARRAY", ua, ua_n);
        }
        else if (strcmp(cmd, "PRINT_OA") == 0) {
            print_head_no_count("ORDERED ARRAY", oa, oa_n);
        }
        else if (strcmp(cmd, "END") == 0) {
            break;
        }
        else {
            /* 未知命令，为了实验清晰，直接提示一下 */
            printf("UNKNOWN COMMAND: %s\n", cmd);
        }
    }

    return 0;
}