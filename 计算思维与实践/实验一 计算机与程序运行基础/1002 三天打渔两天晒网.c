#include <stdio.h>
#include <stdbool.h>

// 判断是否为闰年
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 获取某个月的天数
int getDaysInMonth(int year, int month) {
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return daysInMonth[month - 1];
}

// 检查日期是否合法
bool isValidDate(int year, int month, int day) {
    if (year < 1990) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > getDaysInMonth(year, month)) return false;
    return true;
}

// 计算从1990-01-01到指定日期的总天数
int calculateTotalDays(int year, int month, int day) {
    int totalDays = 0;
    
    // 计算1990年到目标年份前一年的总天数
    for (int y = 1990; y < year; y++) {
        totalDays += isLeapYear(y) ? 366 : 365;
    }
    
    // 计算目标年份已过去的天数
    for (int m = 1; m < month; m++) {
        totalDays += getDaysInMonth(year, m);
    }
    
    // 加上当前月的天数
    totalDays += day;
    
    return totalDays;
}

int main() {
    int year, month, day;
    
    // 读取输入
    if (scanf("%4d-%2d-%2d", &year, &month, &day) != 3) {
        printf("Invalid input");
        return 0;
    }
    // 验证输入日期
    if (!isValidDate(year, month, day)) {
        printf("Invalid input");
        return 0;
    }
    // 计算总天数
    int totalDays = calculateTotalDays(year, month, day);
    // 判断状态（5天一个周期：3天打渔，2天晒网）
    int cycleDay = (totalDays - 1) % 5;  // -1因为从1990-01-01开始算第一天

    if (cycleDay < 3) {
        printf("working");
    } else {
        printf("rest");
    }
    return 0;
}