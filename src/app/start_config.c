// root用户用来配置权限数据库的入口程序
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/*引入连接Mysql的头文件*/
#include "/usr/include/mysql/mysql.h"

// 定义布尔类型
#ifndef __cplusplus
 
typedef char bool;
#define false 0
#define true  1
 
#endif

bool is_root()
{
    uid_t my_uid;
    my_uid = getuid();
    if (my_uid==0)
        return true;
    else
        return false;
}

void login_proc()
{
    printf("Welcome to proctorEXE, your program authority management system.\n");

    // 验证root身份
    if (!is_root()){
        printf("This application can only be used by super user. Please contact root user of this device.\n");
        return;
    }

    printf("Please enter command(enter 'help' for help)> ");
    char buffer[256];
    scanf("%s", buffer);
    printf("%s\n",buffer);
}

int main()
{
    login_proc();
    return 0;
}