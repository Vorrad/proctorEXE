/*
定义了与mysql数据库进行交互的接口
*/
#include "../general.h"

/*引入连接Mysql的头文件*/
#include "/usr/include/mysql/mysql.h"

// MySQL服务器信息
// 可自定义此部分
#define HOST "localhost"            /*MySql服务器地址*/
#define USERNAME "root"             /*root用户名*/
#define PASSWORD NULL               /*数据库连接密码，root用户默认不需要密码*/
#define DATABASE "proctorEXE_db"    /*数据库名*/
#define TABLE_PROGRAM "program"     /*数据表*/
#define TABLE_POLICY "policy"       /*数据表*/
#define TABLE_AUDIT "audit"         /*数据表*/

// 函数定义

// 登录数据库
int login_to_db(MYSQL* mysql, const char* host, const char* username, const char* password);

// 连接数据库（全过程）
int connect_to_db(MYSQL* mysql, const char* db_name);

// 检查数据库是否已经存在
bool db_exists(MYSQL* mysql, const char* db_name);

// 从名称创建数据库
int create_db(MYSQL* mysql, const char* db_name);

// 使用数据库
int use_db(MYSQL* mysql, const char* db_name);

// 检查表是否存在
bool table_exists(MYSQL* mysql, const char* table_name, MYSQL_RES** result);

// 从名称创建表
int create_table(MYSQL* mysql, const char* table_name, const char* columns);
