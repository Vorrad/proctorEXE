#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

// 定义布尔类型
#ifndef __cplusplus
 
typedef char bool;
#define false 0
#define true  1
 
#endif

#define BUFFER_SIZE 4096            /*查询语句最大长度*/
#define PARA_SIZE 256               /*参数最大长度*/

#define CREATE 1