// root用户用来配置权限数据库的入口程序
#include "../../include/general.h"
#include "../../include/app/mysql.h"
#include "../../include/app/typedef.h"


bool is_root()
{
    uid_t my_uid;
    my_uid = getuid();
    if (my_uid==0)
        return true;
    else
        return false;
}

bool db_exists(MYSQL* mysql, const char* db_name, MYSQL_RES** result)
{
    char buffer[BUFFER_SIZE];
    bool flag;

    // 查询数据库是否存在
    strcpy(buffer, "SHOW DATABASES LIKE \"" );
    strcat(buffer, db_name);
    strcat(buffer,"\"");
    mysql_real_query(mysql, buffer, (u_long) strlen(buffer));
    *result = mysql_store_result(mysql);
    if (*result != NULL)
        return true;
    else
        return false;
    
}

int create_db(MYSQL* mysql, const char* db_name)
{
    char buffer[BUFFER_SIZE];
    int flag;

    strcpy(buffer, "CREATE DATABASE ");
    strcat(buffer, db_name);
    flag = mysql_real_query(mysql, buffer, (u_long) strlen(buffer));

    return flag;

}

int use_db(MYSQL* mysql, const char* db_name)
{
    char buffer[BUFFER_SIZE];
    int flag;

    strcpy(buffer, "USE ");
    strcat(buffer, db_name);
    flag = mysql_real_query(mysql, buffer, (u_long) strlen(buffer));

    return flag;

}

void connect_to_db(MYSQL* mysql, const char* db_name, MYSQL_RES** result)
{
    // 连接到数据库
    bool flag = db_exists(mysql, db_name, result);
    if (flag)
    {
        long n = (long) mysql_num_rows(*result);       // 查询结果行数
        int res;                                    // 状态码
        switch (n)
        {
        case 0:                                     // 创建数据库
            printf("数据库不存在，创建数据库...");
            res = create_db(mysql, DATABASE);
            if (res == 0)
                printf("创建成功\n");
            else
            {
                printf("数据库创建过程中出错，错误码：%d ，已退出\n", res);
                mysql_close(mysql);
                return;
            }
        default:                                    // 连接到数据库
            res = use_db(mysql, DATABASE);           
            if (res == 0)
                printf("数据库连接成功\n");
            else
            {
                printf("数据库连接过程中出错，错误码：%d ，已退出\n", res);
                mysql_close(mysql);
                return;
            }                
            break;
        }
    }
    else
    {
        printf("rslt is NULL!\n");
        mysql_close(mysql);
        return;
    }
}

void login_proc()
{
    printf("Welcome to proctorEXE, your program authority management system.\n");

    // 验证root身份
    if (!is_root()){
        printf("This application can only be used by super user. Please contact root user of this device.\n");
        return;
    }

    // 连接数据库
    MYSQL connection;                   /*数据库连接*/ 
    int res;                            /*执行sql语句后的返回标志*/ 
    MYSQL_RES* rslt;                    /*语句执行结果*/ 
    MYSQL_ROW result_row;               /*按行返回查询信息*/ 
    int row, column;                    /*定义行数,列数*/
    char query_buffer[BUFFER_SIZE];     /*保存SQL查询语句*/

    mysql_init(&connection);
    // 登陆MySQL服务
    if (mysql_real_connect(&connection, HOST, USERNAME, PASSWORD, NULL, 0, NULL, 0)) {
        printf("MySQL服务器连接成功!\n");

        connect_to_db(&connection, DATABASE, &rslt);
    }
    else
    {
        printf("数据库连接错误，请检查用户名及密码配置\n");
        return;
    }
    // printf("Please enter command(enter 'help' for help)> ");
    // char buffer[256];
    // scanf("%s", buffer);
    // printf("%s\n",buffer);

    // 关闭数据库连接
    mysql_close(&connection);
}

int main()
{
    login_proc();
    return 0;
}