#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 歌曲节点结构体
typedef struct Song {
    int id;            // 歌曲标识，便于定位，用途：简化用户交互
    char title[100];   // 歌曲标题，支持中英文长歌名
    char artist[50];   // 歌曲作者
    char filepath[300];// 文件路径
    struct Song* prev; // 前驱节点的指针
    struct Song* next; // 后继节点的指针
} Song;

// 播放列表管理器
typedef struct PlaylistManager{
    Song* head;        // 指向链表首节点的指针
    Song* tail;        // 指向链表尾节点的指针
    Song* current;     // 指向当前播放歌曲的指针
    int song_count;    // 链表中歌曲总数
} PlaylistManager;

// 函数声明
void init_playlist_manager(PlaylistManager* manager);                           // 初始化链表
int load_songs_from_file(PlaylistManager* manager, const char* filename);       // 从文件中读取到链表
void add_song(PlaylistManager* manager, const char* title, const char* artist,  // 人工增加音乐
            const char* filepath);
int delete_song_by_title(PlaylistManager* manager, const char* title);          // 删除指定名字的音乐
int play_song_by_title(PlaylistManager* manager, const char* title);            // 根据名字播放音乐
void display_playlist(PlaylistManager* manager);                                // 显示播放列表（正向）
void display_playlist_reverse(PlaylistManager* manager);                        // 显示播放列表（反向）
int export_playlist(PlaylistManager* manager, const char* filename);            // 导出歌单
void next_song(PlaylistManager* manager);                                       // 下一首歌
void previous_song(PlaylistManager* manager);                                   // 上一首歌
int play_song_random(PlaylistManager* manager);                                 // 随机播放音乐（非必做）
int insert_song_at(PlaylistManager* manager, int position, const char* title, const char* artist, const char* filepath);    // 向指定位置添加音乐（非必做）
void clear_playlist(PlaylistManager* manager);                                  // 清空播放列表（非必做）
void sort_by_title(PlaylistManager* manager);                                   // 按照歌曲名排序（非必做）

// 辅助函数声明
Song* create_song_node(const char* title, const char* artist, const char* filepath);
void add_node_to_tail(PlaylistManager* manager, Song* new_song);
void renumber_songs(PlaylistManager* manager);
Song* find_song_by_title(PlaylistManager* manager, const char* title);
void remove_node_from_list(PlaylistManager* manager, Song* node_to_remove);
void swap_song_data(Song* a, Song* b);
void display_playlist_generic(PlaylistManager* manager, int reverse);

//==============================================================================
// 1. 创建新节点的通用函数
Song* create_song_node(const char* title, const char* artist, const char* filepath) {
    Song* song = (Song*)malloc(sizeof(Song));
    if (!song) {
        printf("内存分配失败！\n");
        return NULL;
    }
    
    // 初始化节点字段
    memset(song, 0, sizeof(Song));  // 清零所有字段
    strncpy(song->title, title, sizeof(song->title) - 1);
    song->title[sizeof(song->title) - 1] = '\0';
    strncpy(song->artist, artist, sizeof(song->artist) - 1);
    song->artist[sizeof(song->artist) - 1] = '\0';
    strncpy(song->filepath, filepath, sizeof(song->filepath) - 1);
    song->filepath[sizeof(song->filepath) - 1] = '\0';
    
    return song;
}

// 2. 将节点添加到链表尾部的通用函数
void add_node_to_tail(PlaylistManager* manager, Song* new_song) {
    if (!new_song) return;
    
    // 设置新节点的ID
    new_song->id = manager->song_count + 1;
    
    // 处理空链表情况
    if (manager->head == NULL) {
        manager->head = new_song;
        manager->tail = new_song;
        manager->current = new_song;
        new_song->prev = NULL;
        new_song->next = NULL;
    } else {
        // 非空链表，添加到尾部
        new_song->prev = manager->tail;
        new_song->next = NULL;
        manager->tail->next = new_song;
        manager->tail = new_song;
    }
    
    manager->song_count++;
}

// 3. 重新编号所有歌曲的通用函数
void renumber_songs(PlaylistManager* manager) {
    Song* current = manager->head;
    int id = 1;
    while (current != NULL) {
        current->id = id++;
        current = current->next;
    }
}

// 4. 查找歌曲节点的通用函数（按标题，返回第一个匹配的节点）
Song* find_song_by_title(PlaylistManager* manager, const char* title) {
    if (!manager->head || !title) return NULL;
    
    Song* current = manager->head;
    while (current != NULL) {
        if (strcmp(current->title, title) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// 5. 删除节点的通用函数（处理链表指针更新）
void remove_node_from_list(PlaylistManager* manager, Song* node_to_remove) {
    if (!node_to_remove) return;
    
    // 处理前驱节点
    if (node_to_remove->prev) {
        node_to_remove->prev->next = node_to_remove->next;
    } else {
        // 删除的是头节点
        manager->head = node_to_remove->next;
    }
    
    // 处理后继节点
    if (node_to_remove->next) {
        node_to_remove->next->prev = node_to_remove->prev;
    } else {
        // 删除的是尾节点
        manager->tail = node_to_remove->prev;
    }
    
    // 如果删除的是当前播放歌曲，调整current指针
    if (manager->current == node_to_remove) {
        manager->current = node_to_remove->next;
        if (!manager->current) {
            manager->current = manager->head;  // 回退到头部
        }
    }
    
    free(node_to_remove);
    manager->song_count--;
}

// 6. 通用显示播放列表函数（支持正向和反向）
void display_playlist_generic(PlaylistManager* manager, int reverse) {
    if (!manager->head) {
        printf("播放列表为空\n");
        return;
    }
    
    printf("\n播放列表（共%d首歌曲，%s遍历）:\n", manager->song_count, reverse ? "反向" : "正向");
    printf("==========================================\n");
    printf("ID\t标题\t\t\t艺术家\t\t\t文件路径\n");
    printf("-----------------------------------------------------------\n");
    
    Song* current = reverse ? manager->tail : manager->head;
    
    while (current != NULL) {
        printf("%d\t%s\t\t%s\t\t%s\n", 
            current->id, 
            current->title, 
            current->artist, 
            current->filepath);
        current = reverse ? current->prev : current->next;
    }
    printf("==========================================\n");
}
//============================================================================================

// Windows 版本播放函数
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

// 初始化播放管理器
void init_playlist_manager(PlaylistManager* manager){
    manager->head = NULL;
    manager->tail = NULL;
    manager->current = NULL;
    manager->song_count = 0;
}

// 从文件读取歌曲
int load_songs_from_file(PlaylistManager* manager, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("无法打开文件: %s\n", filename);
        return -1;
    }
    
    char line[500];
    int songs_loaded = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        char* title = strtok(line, ",");
        char* artist = strtok(NULL, ",");
        char* filepath = strtok(NULL, ",");
        
        if (!title || !artist || !filepath) {
            printf("跳过格式错误的行: %s\n", line);
            continue;
        }
        
        Song* song = create_song_node(title, artist, filepath);
        if (song){
            add_node_to_tail(manager, song);
            songs_loaded++;
        }
    }
    
    fclose(file);
    printf("从文件加载了 %d 首歌曲\n", songs_loaded);
    return songs_loaded;
}

// 在链表尾部添加歌曲
void add_song(PlaylistManager* manager, const char* title, const char* artist, const char* filepath) {
    Song* new_song = create_song_node(title, artist, filepath);
    if (new_song) {
        add_node_to_tail(manager, new_song);
        printf("成功添加歌曲：%s - %s\n", title, artist);
    }
}

// 按标题删除歌曲
int delete_song_by_title(PlaylistManager* manager, const char* title) {
    if (!manager->head) {
        printf("播放列表为空\n");
        return 0;
    }
    
    int deleted_count = 0;
    Song* current = manager->head;
    
    while (current != NULL) {
        Song* next = current->next;  // 先保存下一个节点
        
        if (strcmp(current->title, title) == 0) {
            remove_node_from_list(manager, current);
            deleted_count++;
        }
        
        current = next;
    }
    
    if (deleted_count > 0) {
        renumber_songs(manager);
        printf("删除了 %d 首歌曲\n", deleted_count);
    } else {
        printf("未找到标题为 '%s' 的歌曲\n", title);
    }
    
    return deleted_count;
}

// 播放歌曲
int play_song_by_title(PlaylistManager* manager, const char* title) {
    Song* song = find_song_by_title(manager, title);
    
    if (song) {
        printf("正在播放：%s - %s\n", song->title, song->artist);
        manager->current = song;
        play_audio(song->filepath);
        return 1;
    } else {
        printf("未找到标题为 '%s' 的歌曲\n", title);
        return 0;
    }
}

// 4. 显示播放列表（正向遍历）
void display_playlist(PlaylistManager* manager) {
    display_playlist_generic(manager, 0);
}

// 5. 显示播放列表（反向遍历）
void display_playlist_reverse(PlaylistManager* manager) {
    display_playlist_generic(manager, 1);
}

// 6. 将播放列表保存到文件
int export_playlist(PlaylistManager* manager, const char* filename) {
    FILE* file = fopen(filename, "w");
    if(!file) {
        printf("无法打开文件 %s\n", filename);
        return 1;
    }

    Song* iterator = manager->head;
    while (iterator != NULL){
        fprintf(file, "%s,%s,%s\n", 
                iterator->title, 
                iterator->artist, 
                iterator->filepath);
        iterator = iterator->next;
    }
    fclose(file);
    printf("播放列表已导出到 %s\n", filename);
    return 0;
}

// 7. 下一首
void next_song(PlaylistManager* manager){
    if (manager->current == NULL) {
        if (manager->head != NULL) {
            manager->current = manager->head;
        } else {
            printf("播放列表为空\n");
            return;
        }
    }
    
    if (manager->current->next != NULL) {
        manager->current = manager->current->next;
        printf("正在播放下一首：%s - %s\n", manager->current->title, manager->current->artist);
        play_audio(manager->current->filepath);
    } else {
        printf("已经是最后一首歌\n");
    }
}

// 8. 上一首
void previous_song(PlaylistManager* manager) {
    if (manager->current == NULL) {
        if (manager->tail != NULL) {
            manager->current = manager->tail;
        } else {
            printf("播放列表为空\n");
            return;
        }
    }
    
    if (manager->current->prev != NULL) {
        manager->current = manager->current->prev;
        printf("正在播放上一首：%s - %s\n", manager->current->title, manager->current->artist);
        play_audio(manager->current->filepath);
    } else {
        printf("已经是第一首歌\n");
    }
}

// 9. 随机播放歌曲（非必做）
int play_song_random(PlaylistManager* manager) {
    if (!manager->head) {
        printf("播放列表为空！\n");
        return 0;
    }
    
    srand(time(NULL));
    int random_pos = rand() % manager->song_count;
    
    Song* current = manager->head;
    for (int i = 0; i < random_pos && current != NULL; i++) {
        current = current->next;
    }
    
    if (current) {
        printf("随机播放：%s - %s\n", current->title, current->artist);
        manager->current = current;
        play_audio(current->filepath);
        return 1;
    }
    
    return 0;
}

// 在指定位置插入歌曲（非必做）
int insert_song_at(PlaylistManager* manager, int position, const char* title, const char* artist, const char* filepath) {
    if (position < 1) position = 1;
    
    // 如果位置超出范围或链表为空，添加到尾部
    if (position > manager->song_count || !manager->head) {
        add_song(manager, title, artist, filepath);
        return 1;
    }
    
    Song* new_song = create_song_node(title, artist, filepath);
    if (!new_song) return 0;
    
    // 插入到头部
    if (position == 1) {
        new_song->prev = NULL;
        new_song->next = manager->head;
        manager->head->prev = new_song;
        manager->head = new_song;
    } else {
        // 找到插入位置的前一个节点
        Song* current = manager->head;
        for (int i = 1; i < position - 1 && current != NULL; i++) {
            current = current->next;
        }
        
        if (!current) {
            free(new_song);
            return 0;
        }
        
        // 插入节点
        new_song->prev = current;
        new_song->next = current->next;
        
        if (current->next) {
            current->next->prev = new_song;
        }
        current->next = new_song;
        
        // 更新尾部指针
        if (!new_song->next) {
            manager->tail = new_song;
        }
    }
    
    manager->song_count++;
    renumber_songs(manager);
    
    printf("成功在位置 %d 插入歌曲：%s - %s\n", position, title, artist);
    return 1;
}

// 按歌曲标题排序（非必做）
void sort_by_title(PlaylistManager* manager) {
    if (!manager->head || !manager->head->next) return;
    
    int swapped;
    do {
        swapped = 0;
        Song* current = manager->head;
        
        while (current->next != NULL) {
            if (strcmp(current->title, current->next->title) > 0) {
                // 交换数据而不交换指针
                swap_song_data(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
    
    renumber_songs(manager);
    printf("已按歌曲名排序\n");
}

// 交换两个节点的数据（辅助函数）
void swap_song_data(Song* a, Song* b) {
    // 交换ID
    int temp_id = a->id;
    a->id = b->id;
    b->id = temp_id;
    
    // 交换标题
    char temp_title[100];
    strcpy(temp_title, a->title);
    strcpy(a->title, b->title);
    strcpy(b->title, temp_title);
    
    // 交换艺术家
    char temp_artist[50];
    strcpy(temp_artist, a->artist);
    strcpy(a->artist, b->artist);
    strcpy(b->artist, temp_artist);
    
    // 交换文件路径
    char temp_filepath[300];
    strcpy(temp_filepath, a->filepath);
    strcpy(a->filepath, b->filepath);
    strcpy(b->filepath, temp_filepath);
}

// 清空播放列表（非必做）
void clear_playlist(PlaylistManager* manager) {
    Song* current = manager->head;
    while (current != NULL) {
        Song* next = current->next;
        free(current);
        current = next;
    }
    init_playlist_manager(manager);
    printf("播放列表已清空\n");
}

// 显示菜单
void display_menu() {
    printf("\n");
    printf("链表音乐播放器管理器\n");
    printf("==========================================\n");
    printf("1. 添加歌曲\n");
    printf("2. 删除歌曲 (按标题)\n");
    printf("3. 播放歌曲 (按标题)\n");
    printf("4. 显示播放列表 (正向)\n");
    printf("5. 显示播放列表 (逆向)\n");
    printf("6. 导出歌单\n");
    printf("7. 切换到下一首歌\n");
    printf("8. 切换到上一首歌\n");
    printf("9. 随机播放歌曲(非必做)\n");
    printf("10. 在指定位置添加歌曲(非必做)\n");
    printf("11. 清空播放列表(非必做)\n");
    printf("12. 按照歌曲名排序(非必做)\n");
    printf("0. 退出程序\n");
    printf("==========================================\n");
    printf("请选择操作 (0-12): ");
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

    printf("=== 双向链表音乐播放器管理器 ===\n");
    printf("已加载 %d 首示例歌曲\n", manager.song_count);
    manager.current = manager.head;
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
            case 1: {                   // 添加歌曲
                char title[100], artist[50], filepath[300];
                float duration;

                get_user_input(title, sizeof(title), "请输入歌曲标题: ");
                get_user_input(artist, sizeof(artist), "请输入作者: ");
                get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
                clear_input_buffer();

                add_song(&manager, title, artist, filepath);
                break;
            }
            case 2: {                   // 删除歌曲 (按标题)
                char title[100];
                get_user_input(title, sizeof(title), "请输入要删除的歌曲标题: ");
                int res = delete_song_by_title(&manager, title);
                break;
            }
            case 3: {                   // 播放歌曲（按歌曲名）
                char title[100];
                get_user_input(title, sizeof(title), "请输入要播放的歌曲标题: ");
                int res = play_song_by_title(&manager, title);
                break;
            }
            case 4: {                   // 显示播放列表（正向）
                display_playlist(&manager);
                break;
            }
            case 5: {                   // 显示播放列表（逆向）
                display_playlist_reverse(&manager);
                break;
            }
            case 6: {                   // 导出播放列表
                char path2export[300];
                get_user_input(path2export, sizeof(path2export), "请输入要导出的目标文件名: ");
                int res = export_playlist(&manager, path2export);
                break;
            }
            case 7: {                   // 播放下一首歌曲
                next_song(&manager);
                break;
            }
            case 8: {                   // 播放上一首歌曲
                previous_song(&manager);
                break;
            }
            case 9: {                   // 随机播放歌曲(非必做)
                int res = play_song_random(&manager);
                break;
            }
            case 10: {                  // 在指定位置添加歌曲(非必做)
                char title[100], artist[50], filepath[300];
                int position;
                get_user_input(title, sizeof(title), "请输入歌曲标题: ");
                get_user_input(artist, sizeof(artist), "请输入作者: ");
                get_user_input(filepath, sizeof(filepath), "请输入歌曲路径: ");
                printf("请输入歌曲插入位置: ");
                scanf("%d", &position);
                insert_song_at(&manager, position, title, artist, filepath);
                break;
            }
            case 11: {                  // 清空播放列表(非必做)
                clear_playlist(&manager);
                break;
            }
            case 12: {                  // 按照歌曲名排序(非必做)
                sort_by_title(&manager);
                break;
            }
            case 0: // 退出程序
                printf("感谢使用链表音乐播放器管理器!\n");
                break;
            default:
                printf("? 无效选择，请重新输入\n");
                break;
        }

        // 暂停，让用户看到结果
        if (choice != 0) {
            printf("\n按回车键继续...");
            getchar();
        }

    } while (choice != 0);

    // 清理内存
    clear_playlist(&manager);

    return 0;
}