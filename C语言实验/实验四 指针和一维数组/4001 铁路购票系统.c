#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ROWS 20
#define COLS 5

const char seat[] = {'A', 'B', 'C', 'D', 'F'};
bool SeatState[ROWS][COLS] = {false};

bool assignConSeats(int k, char assignSeat[][5]);
void assignSepSeats(int k, char assignSeat[][5]);

int main() {
    int buyCount;
    int ticketNum[100];

    // 读取购买次数和每次买的票数
    scanf("%d", &buyCount);
    for (int i = 0; i < buyCount; i++){
        scanf("%d", &ticketNum[i]);
    }

    // 按每一次处理购买
    for (int i = 0; i < buyCount; i++){
        int k = ticketNum[i]; // 每一次购买票数
        char assignSeat[5][5]; // 用来储存k次购买每一次的位置

        // 先分配连续座位
        bool foundContinue = assignConSeats(k, assignSeat);
        
        // 如果没有，分配分散座位
        if (!foundContinue) {
            assignSepSeats(k, assignSeat);
        }
        
        // 输出分配结果
        for (int j = 0; j < k; j++){
            if (j != 0){
                printf(" ");
            }
            printf("%s", assignSeat[j]);
        }
        printf("\n");
    }
    return 0;
}

// 分配连续座位的函数
bool assignConSeats(int k, char assignSeat[][5]){
    for (int row = 0; row < ROWS; row++){
        for (int start = 0; start <= COLS - k; start++){
            // 检查是否k个全空
            bool isEmpty = true;
            for (int j = 0; j < k; j++){
                if(SeatState[row][start + j]){
                    isEmpty = false;
                    break;
                }
            }
            if (isEmpty){
                // 分配连续座位
                for (int j = 0; j < k; j++){
                    sprintf(assignSeat[j], "%d%c", row + 1, seat[start + j]);
                    SeatState[row][start + j] = true;
                }
                return true;
            }
        }
    }
    return false;
}

// 分配分散座位的函数
void assignSepSeats(int k, char assignSeat[][5]){
    int assigned = 0;
    for (int row = 0; row < ROWS && assigned < k; row++){
        for (int col = 0; col < COLS && assigned < k; col++){
            if (!SeatState[row][col]){
                sprintf(assignSeat[assigned], "%d%c", row + 1, seat[col]);
                SeatState[row][col] = true;
                assigned++;
            }
        }
    }
}