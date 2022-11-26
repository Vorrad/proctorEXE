#include "../../include/app/mysql.h"

int login_to_db(MYSQL* mysql, const char* host, const char* username, const char* password)
{
    if (mysql_real_connect(mysql, host, username, password, NULL, 0, NULL, 0))
    {
        printf("MySQL服务器连接成功!\n");
        
        int res;
        res = connect_to_db(mysql, DATABASE);
        if (res != 0)
            return res;
            
        return 0;
    }
    else
    {
        printf("数据库连接错误，请检查用户名及密码配置\n");
        return -1;
    }
}

int connect_to_db(MYSQL* mysql, const char* db_name)
{

    // 连接到数据库
    bool flag = db_exists(mysql, db_name);      // 查询数据库是否已存在
    int res;                                    // 状态码

    switch (flag)
    {
    case false:                                     // 创建数据库
        printf("数据库不存在，创建数据库...");
        res = create_db(mysql, DATABASE);
        if (res == 0)
            printf("数据库创建成功，检查数据表中...\n");
            
        //     res = table_exists(mysql, TABLE_PROGRAM, result)
        else
        {
            printf("数据库创建过程中出错，错误码：%d ，已退出\n", res);
            mysql_close(mysql);
            return res;
        }
    default:                                    // 连接到数据库
        res = use_db(mysql, DATABASE);           
        
        if (res == 0)
            printf("数据库连接成功\n");
        else
        {
            printf("数据库连接过程中出错，错误码：%d ，已退出\n", res);
            mysql_close(mysql);
            return res;
        }

        return 0;
    }
}

bool db_exists(MYSQL* mysql, const char* db_name)
{
    MYSQL_RES* result;

    char buffer[BUFFER_SIZE];
    bool flag;

    // 查询数据库是否存在
    strcpy(buffer, "SHOW DATABASES LIKE \"" );
    strcat(buffer, db_name);
    strcat(buffer,"\"");
    
    int res;
    res = mysql_real_query(mysql, buffer, (u_long) strlen(buffer));
    if (res != 0)
        fprintf(stderr,"查询错误: %d\n", res);
    result = mysql_store_result(mysql);
    
    if (result == NULL)
    {
        fprintf(stderr,"查询错误: 未返回结果\n");
        return false;
    }
        

    long n = (long) mysql_num_rows(result);
    printf("n: %ld", n);
    if (!n)
        return false;
    else
        return true;    
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

bool table_exists(MYSQL* mysql, const char* table_name, MYSQL_RES** result)
{
    char buffer[BUFFER_SIZE];
    bool flag;

    // 查询数据库是否存在
    strcpy(buffer, "SHOW TABLES LIKE \"" );
    strcat(buffer, table_name);
    strcat(buffer,"\"");
    mysql_real_query(mysql, buffer, (u_long) strlen(buffer));
    *result = mysql_store_result(mysql);
    if (*result != NULL)
        return true;
    else
        return false;
    
}

int create_table(MYSQL* mysql, const char* table_name, const char* columns)
{
    char buffer[BUFFER_SIZE];
    int flag;

    strcpy(buffer, "CREATE TABLE ");
    strcat(buffer, table_name);
    strcat(buffer, columns);
    flag = mysql_real_query(mysql, buffer, (u_long) strlen(buffer));

    return flag;

}