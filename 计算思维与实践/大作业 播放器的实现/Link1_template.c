#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 歌曲节点结构体
typedef struct Song {
    int id;
    char title[100];
    char artist[50];
    char filepath[300];
    struct Song* next;
} Song;

// 播放列表管理器
typedef struct PlaylistManager{
    Song* head;
    Song* tail;
    Song* current;
    int song_count;
} PlaylistManager;

// 函数声明
void init_playlist_manager(PlaylistManager* manager);                           // 初始化链表
int load_songs_from_file(PlaylistManager* manager, const char* filename);       // 从文件中读取到链表
void add_song(PlaylistManager* manager, const char* title, const char* artist,  // 人工增加音乐
            const char* filepath);
void display_playlist(PlaylistManager* manager);                                // 显示播放列表
int delete_songs_by_title(PlaylistManager* manager, const char* title);         // 删除指定名字的音乐
int play_song_by_title(PlaylistManager* manager, const char* title);            // 根据名字播放音乐
int export_playlist(PlaylistManager* manager, const char* filename);            // 导出播放列表
int play_song_random(PlaylistManager* manager);                                 // 随机播放音乐
int insert_song_at(PlaylistManager* manager, int position, const char* title, const char* artist, const char* filepath);    // 向指定位置添加音乐
void destroy_playlist(PlaylistManager* manager);                                // 清空列表

// linux/Mac 版本
// void play_audio(const char* filename) {
//     char command[256];
//     FILE *mp3File = fopen(filename, "rb");
//     if (!mp3File) {
//         printf("无法打开文件 %s\n", filename);
//         return;
//     }
//     else{
//         printf("Founded File!!");
//     }
//     snprintf(command, sizeof(command), "afplay \"%s\"", filename);
//     int ret = system(command);
//     if (ret != 0) {
//         printf("播放失败或中断，请检查文件格式是否支持。\n");
//     }
// }

// Windows 版本
void play_audio(const char* filename){
    FILE *mp3File = fopen(filename, "rb");
    if (!mp3File) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    fclose(mp3File);
    
    char command[512];
    
    snprintf(command, sizeof(command), "wmplayer \"%s\"", filename);

    printf("正在播放：%s\n", filename);
    int ret = system(command);
    if (ret != 0) {
        printf("播放失败，错误码：%d\n", ret);
        printf("请确保 Windows Media Player 已安装或文件格式正确\n");
    }
}

int load_songs_from_file(PlaylistManager* manager, const char* filename){
    //怎么读：fopen,再用sting.h里的strtok
    //怎么储存：strncpy储存到节点song
    //怎么加：链表的插入

    FILE* file = fopen(filename, "r");
    if (!file){
        return -1;
    }

    char line[100];
    int songs_loaded = 0;

    while(fgets(line, sizeof(line), file)){
        line[strcspn(line, "\n")] = '\0';

        char* title = strtok(line, ",");
        char* artist = strtok(NULL, ",");
        char* filepath = strtok(NULL, ",");

        Song* song = (Song*)malloc(sizeof(Song));
        if (!song) continue;

        song->id = manager->song_count + 1;
        strncpy(song->title, title, sizeof(song->title));
        song->title[sizeof(song->title) - 1] = '\0';
        strncpy(song->artist, artist, sizeof(song->artist));
        song->artist[sizeof(song->artist) - 1] = '\0';
        strncpy(song->filepath, filepath, sizeof(song->filepath));
        song->filepath[sizeof(song->filepath) - 1] = '\0';
        song->next = NULL;

        //添加到播放器里面
        if (manager->head == NULL){
            manager->head = song;
            manager->tail = song;
            manager->current = song;
        }else{
            manager->tail->next = song;
            manager->tail = song;
        }
        
        manager->song_count++;
        songs_loaded++;
    }

    fclose(file);
    return songs_loaded;
}


// 初始化播放管理器
void init_playlist_manager(PlaylistManager* manager){
    manager->head = NULL;
    manager->tail = NULL;
    manager->current = NULL;
    manager->song_count = 0;
}

// 1. 在链表末尾添加歌曲
void add_song(PlaylistManager* manager, const char* title, const char* artist, const char* filepath) {
    Song* song = (Song*)malloc(sizeof(Song));
    if(!song) return;

    song->id = manager->song_count + 1;
    strncpy(song->title, title, sizeof(song->title));
    song->title[sizeof(song->title) - 1] = '\0';
    strncpy(song->artist, artist, sizeof(song->artist));
    song->artist[sizeof(song->artist) - 1] = '\0';
    strncpy(song->filepath, filepath, sizeof(song->filepath));
    song->filepath[sizeof(song->filepath) - 1] = '\0';
    song->next = NULL;

    //添加到播放器里面
    if (manager->head == NULL){
        manager->head = song;
        manager->tail = song;
        manager->current = song;
    }else{
        manager->tail->next = song;
        manager->tail = song;
    }
    
    manager->song_count++;
    return;
}

// 2. 显示播放列表
void display_playlist(PlaylistManager* manager){
    if(manager->head == NULL){
        printf("播放列表为空\n");
        return;
    }
    printf("\n播放列表（共%d首歌曲）:\n", manager->song_count);
    printf("==========================================\n");
    printf("ID\t标题\t\t\t艺术家\t\t\t文件路径\n");
    printf("-----------------------------------------------------------\n");

    Song* current = manager->head;
    while (current != NULL) {
        printf("%d\t%s\t\t%s\t\t%s\n", 
            current->id, 
            current->title, 
            current->artist, 
            current->filepath);
        current = current->next;
    }
    printf("==========================================\n");
    return;
}

// 3. 删除歌曲
int delete_songs_by_title(PlaylistManager* manager, const char* title){
    //按顺序找，找一个删一个（不止一个）
    //删除使用双指针
    //注意特殊情况和处理更新

    if(manager->head == NULL){
        printf("播放列表为空\n");
        return 0;
    }

    Song* iterator = manager->head;
    Song* prev = NULL;
    int deleted_count = 0;

    while(iterator != NULL){
        if (strcmp(iterator->title, title) == 0){
            Song* delete = iterator;
            //头结点
            if (prev == NULL){
                manager->head = iterator->next;
            }else{
                prev->next = iterator->next;
            }

            //尾结点
            if (manager->tail == iterator){
                manager->tail = prev;
            }
            
            //播放节点
            if (manager->current == iterator){
                manager->current = iterator->next;
            }

            iterator = iterator->next;
            free(delete);
            
            manager->song_count--;
            deleted_count++;
        }else{
            prev = iterator;
            iterator = iterator->next;
        }
    }

    // 重新编号
    if (deleted_count > 0) {
        Song* iterator2 = manager->head;
        int id = 1;
        while(iterator2 != NULL){
            iterator2->id = id;
            id++;
            iterator2 = iterator2->next;
        }
    }

    printf("删除了 %d 首歌曲\n", deleted_count);
    
    return deleted_count;
}

// 4. 播放歌曲
int play_song_by_title(PlaylistManager* manager, const char* title){
    if(manager->head == NULL){
        printf("播放列表为空\n");
        return 0;
    }

    Song* iterator = manager->head;
    while (iterator != NULL) {
        if (strcmp(iterator->title, title) == 0) {
            printf("正在播放：%s - %s\n", iterator->title, iterator->artist);
            manager->current = iterator;
            play_audio(iterator->filepath);
            return 1;
        }
        iterator = iterator->next;
    }
    printf("未找到标题为 '%s' 的歌曲\n", title);
    return 0;
}

// 5. 将播放列表保存到文件
int export_playlist(PlaylistManager* manager, const char* filename){
    FILE* file = fopen(filename, "w");
    if(!file) return 1;

    Song* iterator = manager->head;
    while (iterator != NULL){
        fprintf(file, "%s,%s,%s\n", 
                iterator->title, 
                iterator->artist, 
                iterator->filepath);
            
        iterator = iterator->next;
    }
    fclose(file);
    return 0;
}

// 6. 随机播放歌曲（非必做）
int play_song_random(PlaylistManager* manager) {
    if (manager->head == NULL) {
        printf("播放列表为空！\n");
        return 0;
    }

    //种子随机(这个幸亏我写过^*^)
    srand(time(NULL));

    int random_pos = rand() % manager->song_count;

    Song* iterator = manager->head;
    for (int i = 0; i < random_pos && iterator != NULL; i++) {
        iterator = iterator->next;
    }
    
    if (iterator != NULL) {
        printf("随机播放：%s - %s\n", iterator->title, iterator->artist);
        manager->current = iterator;
        play_audio(iterator->filepath);
        return 1;
    }

    return 0;
}

// 7. 在指定位置插入歌曲（非必做）
int insert_song_at(PlaylistManager* manager, int position, const char* title, 
                const char* artist, const char* filepath){
    if (position < 1) {
        position = 1;  // 如果位置小于1，插入到最前面
    }

    if (position > manager->song_count || manager->head == NULL) {
        add_song(manager, title, artist, filepath);
        return 1;
    }
    
    // 创建新节点
    Song* song = (Song*)malloc(sizeof(Song));
    if (!song) {
        return 0;
    }
    
    // 设置新节点属性
    song->id = manager->song_count + 1;
    strncpy(song->title, title, sizeof(song->title));
    song->title[sizeof(song->title) - 1] = '\0';
    strncpy(song->artist, artist, sizeof(song->artist));
    song->artist[sizeof(song->artist) - 1] = '\0';
    strncpy(song->filepath, filepath, sizeof(song->filepath));
    song->filepath[sizeof(song->filepath) - 1] = '\0';
    song->next = NULL;

    
    // 在指定位置插入
    if (position == 1) {
        // 插入到头部
        song->next = manager->head;
        manager->head = song;
        // 更新 tail 如果之前列表为空
        if (manager->tail == NULL) {
            manager->tail = song;
        }
    } else {
        // 找到插入位置的前一个节点
        Song* prev = manager->head;
        for (int i = 1; i < position - 1; i++) {
            prev = prev->next;
        }
        
        song->next = prev->next;
        prev->next = song;

        // 如果插入到末尾，更新 tail
        if (song->next == NULL) {
            manager->tail = song;
        }
    }
    
    manager->song_count++;
    
    // 重新编号所有歌曲
    Song* current = manager->head;
    int id = 1;
    while (current != NULL) {
        current->id = id++;
        current = current->next;
    }
    
    printf("成功在位置 %d 插入歌曲：%s - %s\n", position, title, artist);
    return 1;
}

// 8. 销毁整个链表（非必做）
void destroy_playlist(PlaylistManager* manager) {
    Song* iterator = manager->head;
    while (iterator != NULL) {
        Song* next = iterator->next;
        free(iterator);
        iterator = next;
    }
    init_playlist_manager(manager);
    printf("播放列表已清空\n");
}

void display_menu() {
    printf("\n");
    printf("链表音乐播放器管理器\n");
    printf("==========================================\n");
    printf("1. 人工添加歌曲\n");
    printf("2. 显示播放列表\n");
    printf("3. 删除歌曲 (按标题)\n");
    printf("4. 播放歌曲 (按标题)\n");
    printf("5. 导出歌单\n");
    printf("6. 随机播放歌曲(非必做)\n");
    printf("7. 在指定位置添加歌曲(非必做)\n");
    printf("8. 清空播放列表(非必做)\n");
    printf("0. 退出程序\n");
    printf("==========================================\n");
    printf("请选择操作 (0-8): ");
}



// 清除输入缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 获取用户输入的字符串
void get_user_input(char* buffer, int size, const char* prompt) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    // 去除换行符
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    }
}

// 主函数 - 交互式程序
int main() {
    PlaylistManager manager;
    init_playlist_manager(&manager);
    load_songs_from_file(&manager,"song_list.txt");

    printf("=== 链表音乐播放器管理器 ===\n");
    printf("已加载 %d 首示例歌曲\n", manager.song_count);

    int choice;
    char input[100];

    do {
        display_menu();

        if (scanf("%d", &choice) != 1) {
            printf("无效输入，请输入数字\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: {               // 添加歌曲
                char title[100], artist[50], filepath[300];

                get_user_input(title, sizeof(title), "请输入歌曲标题: ");
                get_user_input(artist, sizeof(artist), "请输入作者: ");
                get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
                clear_input_buffer();

                add_song(&manager, title, artist, filepath);
                break;
            }
            case 2:{                // 显示播放列表 (正向)
                display_playlist(&manager);
                break;
            }
            case 3: {               // 删除歌曲 (按标题)
                char title[100];
                get_user_input(title, sizeof(title), "请输入要删除的歌曲标题: ");
                delete_songs_by_title(&manager, title);
                break;
            }
            case 4: {                 // 按歌曲名播放歌曲
                char title[100];
                get_user_input(title, sizeof(title), "请输入要播放的歌曲标题: ");
                int res = play_song_by_title(&manager, title);
                break;
            }
            case 5: {
                char path2export[300];
                get_user_input(path2export, sizeof(path2export), "请输入要导出的目标文件名: ");
                export_playlist(&manager, path2export);
                break;
            }
            case 6: {
                play_song_random(&manager);
                break;
            }
            case 7: {
                char title[100], artist[50], filepath[300];
                int position;
                get_user_input(title, sizeof(title), "请输入歌曲标题: ");
                get_user_input(artist, sizeof(artist), "请输入作者: ");
                get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
                printf("请输入歌曲插入位置: ");
                scanf("%d", &position);
                clear_input_buffer();
                insert_song_at(&manager, position, title, artist, filepath);
                break;
            }
            case 8: {
                destroy_playlist(&manager);
                break;
            }
            case 0: // 退出程序
                printf("感谢使用链表音乐播放器管理器!\n");
                break;

            default:
                printf("无效选择，请重新输入\n");
                break;
        }

        // 暂停，让用户看到结果
        if (choice != 0) {
            printf("\n按回车键继续...");
            getchar();
        }

    } while (choice != 0);

    // 清理内存
    destroy_playlist(&manager);

    return 0;
}
