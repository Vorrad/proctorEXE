#include "../app/mysql.h"

/*
    @param pid: 目标程序的pid
    @return 是否具有权限，0为无权，1为有权
*/
int ret_auth(pid_t pid);