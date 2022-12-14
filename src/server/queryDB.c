#include "../../include/server/queryDB.h"

int ret_auth(pid_t pid, int action)
{
    char path[PATH_SIZE];
    int res;

    /* 连接数据库 */
    MYSQL connection;                   /*数据库连接*/ 

    mysql_init(&connection);
    // 登陆MySQL服务
    login_to_db(&connection, HOST, USERNAME, PASSWORD);

    /* 获取pid对应的程序路径 */
    if (res = get_path(pid, path))
    {
        printf("fail to get process path.\n");
        mysql_close(&connection);
        return -1;
    }

    printf("PATH: %s\n", path);
    
    dirList* head, *node;

    head = path_to_dirList(path);

    // 遍历层级路径表，通过路径匹配程序
    node = head;
    bool flag = false;
    int prog_id;
    while(1)
    {
        node = node->next;
        
        char dirname[PATH_SIZE];
        strcpy(dirname, node->name);

        char query_buf[BUFFER_SIZE];
        sprintf(query_buf, "SELECT id FROM program WHERE path LIKE \"%s\"", dirname);
        
        int res;
        res = mysql_real_query(&connection, query_buf, (u_long) strlen(query_buf));
        if (res) {
            printf("MySQL query error: %s\n",mysql_error(&connection));
            return -1;
        }
        
        MYSQL_RES* result;
        result = mysql_store_result(&connection);
        long row;
        row = (long) mysql_num_rows(result);

        // 匹配到结果,跳出循环
        if(row)
        {
            MYSQL_ROW result_row;
            result_row = mysql_fetch_row(result);

            prog_id = atoi(result_row[0]);
            flag = true;

            mysql_free_result(result);
            break;
        }

        mysql_free_result(result);

        // 到达链表尾部,退出循环
        if (node->next == NULL)
            break;
    }

    if(flag)    // 如果匹配到了路径
    {
        char query_buf[BUFFER_SIZE];
        sprintf(query_buf, "SELECT auth FROM policy WHERE prog_id=%d AND action=\"%s\"", prog_id, switchAction(action));
        printf("querybuf: %s\n", query_buf);

        int res;
        res = mysql_real_query(&connection, query_buf, (u_long) strlen(query_buf));
        if (res) {
            printf("MySQL query error: %s\n",mysql_error(&connection));
            return -1;
        }
        
        MYSQL_RES* result;
        result = mysql_store_result(&connection);
        long row;
        row = (long) mysql_num_rows(result);

        // 有相关结果
        if(row)
        {
            MYSQL_ROW result_row;
            int auth;
            result_row = mysql_fetch_row(result);

            if (strcmp(result_row[0], "Y") == 0)
                auth = 1;
            else if (strcmp(result_row[0], "N") == 0)
                auth = 0;
            else
                auth = -1;

            mysql_free_result(result);
            return auth;
        }

        // 无结果,返回默认值
        mysql_free_result(result);
        return DEFAULT_AUTH;
    }
    else
    {
        return DEFAULT_AUTH;
    }

    mysql_close(&connection);

    return 1;
}

int get_path(pid_t pid, char* path)
{
    char cmd[PATH_SIZE];    

    sprintf(cmd, "ls -al /proc/%d | grep exe | awk '{print $NF}' > ./%s", pid, SYSOUT);
    system(cmd);

    FILE* fp;
    if ((fp = fopen(SYSOUT,"r")) == NULL)
    {
        fprintf(stderr, "help: fail to get program path from pid\n");
        return -1;
    }

    int i = 0;
    while(1)
    {
        char c;
        c = fgetc(fp);
        if(c=='\n')
        {
            path[i] = '\0';
            break;
        }
        else
        {
            path[i] = c;
            i++;
        }
    }


    fclose(fp);
    remove(SYSOUT);

    return 0;
}

dirList* path_to_dirList(const char* path)
{
    char path_cp[PATH_SIZE];
    strcpy(path_cp, path);
    char *temp = strtok(path_cp,"/");
    
    dirList *head, *node;
    head = (dirList*) malloc(sizeof(dirList));
    head->name = "";
    head->next = NULL;

    char* lastname = "";
    while(temp)
    {
        // 新路径添加到头部
        node = head;
        head = (dirList*) malloc(sizeof(dirList));
        head->next = node;

        char* newname = (char*) malloc(PATH_SIZE);
        sprintf(newname, "%s/%s", lastname, temp);
        node->name = newname;
        lastname = newname;

        temp = strtok(NULL,"/");
    }

    return head;
}
