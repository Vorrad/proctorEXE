// root用户用来配置权限数据库的入口程序
#include "../../include/general.h"
#include "../../include/app/mysql.h"
#include "../../include/app/operation.h"
#include "../../include/server/queryDB.h"


bool is_root()
{
    uid_t my_uid;
    my_uid = getuid();
    if (my_uid==0)
        return true;
    else
        return false;
}

void test()
{

    // 验证root身份
    if (!is_root()){
        printf("This server can only be started by super user. Please contact root user of this device.\n");
        return;
    }

    ret_auth(getpid(), 1);

    // // 连接数据库
    // MYSQL connection;                   /*数据库连接*/ 

    // mysql_init(&connection);
    // // 登陆MySQL服务
    // login_to_db(&connection, HOST, USERNAME, PASSWORD);
    
    // // 关闭数据库连接
    // mysql_close(&connection);
}

int main()
{
    test();
    return 0;
}