/*
定义了用户操作接口
*/
#include "mysql.h"
#include "../general.h"

#define HELP_FILE "./doc/help.txt"
#define ACTION_LIST "./doc/actions.txt"

// 用户操作主循环，用于持续接收用户输入并发送相应指令
void control_panel();

const char* switchAction(int i);

// 命令：帮助
void help();

// 命令：显示
void show(MYSQL* mysql);
void showRules(MYSQL* mysql);
void showPrograms(MYSQL* mysql);
void showLogs(MYSQL* mysql);

// 命令：新增
void add(MYSQL* mysql);
void addRule(MYSQL* mysql);
void addProgram(MYSQL* mysql);

// 命令：更新
void update(MYSQL* mysql);

// 命令：删除
void delete(MYSQL* mysql);

// 未知命令
void unknown_cmd();
