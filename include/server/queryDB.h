#include "../app/mysql.h"
#include "../app/operation.h"

#define SYSOUT "sysout.txt"     // 执行系统命令后输出保存的文件

// 单链表表示下的层级路径表
typedef struct dir{
    char* name;
    struct dir *next;
} dirList;

/*
    @param pid: 目标程序的pid
    @param action: 动作对应的索引
    @return 是否具有权限，0为无权，1为有权，-1为出错
*/
int ret_auth(pid_t pid, int action);

/*
    @param pid: 目标程序的pid
    @param path: 用于保存目标程序的路径
    @return 执行结果，0为正常，-1为出错
*/
int get_path(pid_t pid, char* path);

/*
    @param path: 需要识别的总路径
    @return 转化后的层级路径表
*/
dirList* path_to_dirList(const char* path);