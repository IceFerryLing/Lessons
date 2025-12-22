#include <stdio.h>
#include <stdbool.h>

// 递归函数，计算当前玩家在子数组[left, right]能获得的最大分数优势
int dfs(int* nums, int left, int right) {
    // 如果只剩一个元素，当前玩家只能取这个元素
    if (left == right) {
        return nums[left];
    }
    
    // 当前玩家有两种选择：
    // 1. 取左端元素，然后对手在[left+1, right]区间操作
    // 2. 取右端元素，然后对手在[left, right-1]区间操作
    // 当前玩家会选择让自己优势最大的方案
    int takeLeft = nums[left] - dfs(nums, left + 1, right);
    int takeRight = nums[right] - dfs(nums, left, right - 1);
    
    return takeLeft > takeRight ? takeLeft : takeRight;
}

bool predictTheWinner(int* nums, int numsSize) {
    // 如果玩家1的分数优势 >= 0，则玩家1获胜（包括平局）
    return dfs(nums, 0, numsSize - 1) >= 0;
}

int main() {
    int n;
    scanf("%d", &n);
    
    int nums[20];  // 题目说n<=20
    for (int i = 0; i < n; i++) {
        scanf("%d", &nums[i]);
    }
    
    if (predictTheWinner(nums, n)) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    
    return 0;
}