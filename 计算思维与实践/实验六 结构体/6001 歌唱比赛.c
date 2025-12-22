#include <stdio.h>

#define MAX_STUDENTS 100
#define MAX_JUDGES 20

struct student{
    int scores[MAX_JUDGES];
    double average;
};

void readScores(struct student students[], int numStudent, int JudgeNum);
void calculateScores(struct student students[], int numStudent, int JudgeNum);
int findBestStudent(struct student students[], int numStudent);

int main(){
    int numStudent, JudgeNum;
    struct student students[MAX_STUDENTS];

    scanf("%d %d", &numStudent, &JudgeNum);
    readScores(students, numStudent, JudgeNum);
    calculateScores(students, numStudent, JudgeNum);
    double bestAverage = findBestStudent(students, numStudent);

    printf("%.2f\n", bestAverage);

    return 0;
}

void readScores(struct student students[], int numStudent, int JudgeNum) {
    for (int i = 0; i < numStudent; i++) {
        for (int j = 0; j < JudgeNum; j++) {
            scanf("%d", &students[i].scores[j]);
        }
    }
}

void calculateScores(struct student students[], int numStudent, int JudgeNum) {
    for (int i = 0; i < numStudent; i++) {
        int sum = 0;
        int maxScore = -1;
        int minScore = 11;

        for (int j = 0; j < JudgeNum; j++) {
            int score = students[i].scores[j];
            sum += score;
            if (score > maxScore) maxScore = score;
            if (score < minScore) minScore = score;
        }

        sum -= (maxScore + minScore);
        students[i].average = (double)sum / (JudgeNum - 2);
    }
}

int findBestStudent(struct student students[], int numStudent){
    double maxScores = 0.0;

    for(int i = 0; i < numStudent; i++){
        if(students[i].average > maxScores){
            maxScores = students[i].average;
        }
    }

    return maxScores;
}
