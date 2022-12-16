#include "../../include/server/queryDB.h"

int ret_auth(pid_t pid)
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

    // 遍历层级路径表，匹配路径
    node = head;
    while(1)
    {
        node = node->next;
        printf("node->name: %s\n", node->name);
        if (node->next == NULL)
            break;
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
