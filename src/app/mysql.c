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

    if (!flag)
    {
        printf("数据库不存在，创建数据库...");
        res = create_db(mysql, db_name);
        if (res == 0)
        {
            printf("数据库创建成功，创建数据表中...\n");

            if (res = use_db(mysql,db_name))    // 使用数据库，如果出错则返回
                return res;
            
            res = create_table(mysql, TABLE_PROGRAM, TABLE_PROGRAM_VALUES);

            if (res != 0)
            {
                fprintf(stderr,"数据表创建出错，错误码：%d", res);
                return res;
            }
            printf("数据表program已创建...\n");
            
            res = create_table(mysql, TABLE_POLICY, TABLE_POLICY_VALUES);

            if (res != 0)
            {
                fprintf(stderr,"数据表创建出错，错误码：%d", res);
                return res;
            }
            printf("数据表policy已创建...\n");
            
            res = create_table(mysql, TABLE_AUDIT, TABLE_AUDIT_VALUES);

            if (res != 0)
            {
                fprintf(stderr,"数据表创建出错，错误码：%d", res);
                return res;
            }
            printf("数据表audit已创建...\n");
        }
    }
    else
    {
        if (res = use_db(mysql, db_name))
            return res;                         // 如果出错则返回错误码
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

    if (flag == 0)
    {
        printf("数据库连接成功\n");
        return flag;
    }       
    else
    {
        printf("数据库连接过程中出错，错误码：%d ，已退出\n", flag);
        mysql_close(mysql);
        return flag;
    }
}

int create_table(MYSQL* mysql, const char* table_name, const char* columns)
{
    char buffer[BUFFER_SIZE];
    int flag;

    strcpy(buffer, "CREATE TABLE ");
    strcat(buffer, table_name);
    strcat(buffer, columns);
    flag = mysql_real_query(mysql, buffer, (u_long) strlen(buffer));

    if(flag)
        printf("err: %s\n", mysql_error(mysql));

    return flag;

}