/*引入连接Mysql的头文件*/
#include "/usr/include/mysql/mysql.h"

// MySQL服务器信息
// 可自定义此部分
#define HOST "localhost"            /*MySql服务器地址*/
#define USERNAME "root"             /*root用户名*/
#define PASSWORD NULL               /*数据库连接密码，root用户默认不需要密码*/
#define DATABASE "proctorEXE_db"    /*数据库名*/

// SQL查询参数
#define BUFFER_SIZE 4096            /*查询语句最大长度*/