// root用户用来配置权限数据库的入口程序
#include "../../include/general.h"
#include "../../include/app/mysql.h"
#include "../../include/app/operation.h"
#include "../../include/server/queryDB.h"
#include "user_netlink.h"

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

    int res;
    res = ret_auth(getpid(), 4);

    printf("res: %d\n", res);

    // // 连接数据库
    // MYSQL connection;                   /*数据库连接*/ 

    // mysql_init(&connection);
    // // 登陆MySQL服务
    // login_to_db(&connection, HOST, USERNAME, PASSWORD);
    
    // // 关闭数据库连接
    // mysql_close(&connection);
}

    
int main ()
{
    char buf[1024];
    char msg[1024];
    
    // scanf("%s", msg);
    u2k_socket_init();
    printf("init succees\n");

    // scanf("%s", msg);
    u2k_connect();
    printf("connect!\n");

    while(1)
    {   
        u2k_recv(buf, 1024);
        msg_handle((struct prm_msg *)buf);
        printf("handel finish\n");
    }

    scanf("%s", msg);
    u2k_disconnect();
    printf("disconnect");

    // scanf("%s", msg);
    u2k_socket_release();
    printf("Release!");
    return 0;
}