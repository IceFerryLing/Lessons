#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TXET_LEN 1000
#define MAX_WORD_LEN 100
#define MAX_WORDS 1000

//结构体生成线性表
typedef struct{
    char data[MAX_WORDS][MAX_WORD_LEN];
    int length;
} SeqList;

//结构体初始化
void initList(SeqList *list){
    list->length = 0;
}

int findInList(SeqList* list, char* word){
    for (int i = 0; i < list->length; i++){
        if (strcmp(list->data[i], word) == 0){
            return i;
        }
    }
    return -1;
}

bool insertToList(SeqList* list, char* word){
    if (list->length >= MAX_WORDS){
        return false;
    }

    if (findInList(list, word) == -1){
        strcpy(list->data[list->length++], word);
        return true;
    }
    return false;
}

bool isLetter(char ch){
    return (ch <= 'Z' && ch >= 'A')||(ch <= 'z' && ch >= 'a');
}

bool ReadInWord(char* text, int* index, char* word){
    int i = 0;

    while (text[*index] != '\0' && !isLetter(text[*index])){
        (*index)++;
    }
    while (text[*index] != '\0' && isLetter(text[*index])){
        word[i++] = text[(*index)++];
    }
    if (i > 0){
        word[i] = '\0';
        return true;
    }
    return false;
}

void LowerCase(char* word){
    while (*word){
        if (*word >= 'A' && *word <= 'Z'){
            *word += 32;
        }
        word++;
    }
}

int ProcessText(char* text){
    SeqList wordlist;
    char WordBuffer[MAX_WORD_LEN];
    int index = 0;

    initList(&wordlist);

    while (ReadInWord(text, &index, WordBuffer)){
        LowerCase(WordBuffer);
        insertToList(&wordlist, WordBuffer);
    }
    return wordlist.length;
}

int main(){
    char text[MAX_TXET_LEN];

    fgets(text, sizeof(text),stdin);
    printf("%d", ProcessText(text));
    return 0;
}
