#include "../../include/app/operation.h"

void control_panel(MYSQL* mysql)
{
    char command[PARA_SIZE];

    while (true)
    {
        printf("\nPlease enter command(enter 'help' for help)> ");
        setbuf(stdin, NULL);
        scanf("%s", command);

        if (strcmp(command, "help") * strcmp(command, "h") == 0)
        {
            help();
            continue;
        }
        if (strcmp(command, "exit") * strcmp(command, "quit") * strcmp(command, "e") * strcmp(command, "q") == 0)
        {
            printf("Thanks for your using, bye~\n");
            break;
        }
        if (strcmp(command, "show") * strcmp(command, "s") == 0)
        {
            show(mysql);
            continue;
        }
        if (strcmp(command, "add") * strcmp(command, "a") == 0)
        {
            add(mysql);
            continue;
        }
        if (strcmp(command, "updt") * strcmp(command, "u") == 0)
        {
            update(mysql);
            continue;
        }
        unknown_cmd();
    }
}

const char* switchAction(int i)
{
    if (i < 0 || i > 4)
    {
        fprintf(stderr,"err: %d is not a valid aciton index\n", i);
        return NULL;
    }

    switch (i)
    {
    case 0:
        return "create";

    case 1:
        return "open";

    case 2:
        return "read";

    case 3:
        return "write";

    case 4:
        return "delete";
    }
}

void op_handle(MYSQL* mysql, const char* q)
{
    int res;

    res = mysql_real_query(mysql, q, strlen(q));
    if (res)
    {
        printf("MySQL query error: %s\n",mysql_error(mysql));
        return;
    }
    else
    {
        printf("Operation success.\n");
    }
}

bool query_is_null(MYSQL* mysql, const char* q)
{
    MYSQL_RES* result;
    bool flag;

    int res;
    res = mysql_real_query(mysql, q, (u_long) strlen(q));
    if (res)
    {
        printf("MySQL query error: %s\n",mysql_error(mysql));
        return ERROR;
    }
    result = mysql_store_result(mysql);

    if (result == NULL)
    {
        fprintf(stderr,"query error: result is null\n");
        return ERROR;
    }

    long n = (long) mysql_num_rows(result);
    if (n)
        return false;
    else
        return true;
}

void action_dump()
{
    FILE* fp;
    char read_buf[BUFFER_SIZE];

    if ((fp = fopen(ACTION_LIST, "r")) == NULL)
    {
        printf("err: fail to open action list\n");
        return;
    }

    printf("\nAction list:\n\n");
    while (fgets(read_buf, BUFFER_SIZE, fp) != NULL)
        printf("%s",read_buf);
    
    return;
}

void help()
{
    FILE* fp = NULL;
    char read_buf[BUFFER_SIZE];

    if ((fp = fopen(HELP_FILE, "r")) == NULL)
    {
        printf("help: fail to open filen");
        return;
    }

    printf("\n\n");
    while (fgets(read_buf, BUFFER_SIZE, fp) != NULL)
        printf("%s",read_buf);
    printf("\n\n");

    fclose(fp);
}

void show(MYSQL* mysql)
{
    // 接收参数
    char para[BUFFER_SIZE];
    scanf("%s", para);
    setbuf(stdin, NULL);

    if (strcmp(para, "-r") * strcmp(para, "--rule") == 0)
    {
        showRules(mysql);
        return;
    }
    if (strcmp(para, "-p") * strcmp(para, "--program") == 0)
    {
        showPrograms(mysql);
        return;
    }
    if (strcmp(para, "-a") * strcmp(para, "--audit") == 0)
    {
        showLogs(mysql);
        return;
    }

    printf("Unrecognized command!\n");
    return;
}

void showRules(MYSQL* mysql)
{
    int res;
    char query_buffer[BUFFER_SIZE];
    MYSQL_RES* result;

    strcpy(query_buffer,"SELECT r.id, p.name, r.action, r.auth FROM policy r JOIN program p WHERE r.prog_id=p.id");
    res = mysql_real_query(mysql, query_buffer, (u_long) strlen(query_buffer));

    if (res) {
        printf("MySQL query error: %s\n",mysql_error(mysql));
        return;
    }

    result = mysql_store_result(mysql);
    if (result)
    {
        dump_field(mysql,&result);
        dump_result(mysql,&result);
    }
    else
        printf("result is NULL\n");

    mysql_free_result(result);
    return;
}

void showPrograms(MYSQL* mysql)
{
    int res;
    char query_buffer[BUFFER_SIZE];
    MYSQL_RES* result;

    strcpy(query_buffer,"SELECT * FROM ");
    strcat(query_buffer, TABLE_PROGRAM);
    res = mysql_real_query(mysql, query_buffer, (u_long) strlen(query_buffer));

    if (res) {
        printf("MySQL query error: %s\n",mysql_error(mysql));
        return;
    }

    result = mysql_store_result(mysql);
    if (result)
    {
        printf("Program list:\n");
        dump_field(mysql,&result);
        dump_result(mysql,&result);
    }
    else
        printf("result is NULL\n");

    mysql_free_result(result);
    return;

}

void showLogs(MYSQL* mysql)
{
    int res;
    char query_buffer[BUFFER_SIZE];
    MYSQL_RES* result;

    strcpy(query_buffer,"SELECT a.id, p.name, a.action,a.stat, a.time FROM audit a JOIN program p WHERE a.prog_id=p.id");
    res = mysql_real_query(mysql, query_buffer, (u_long) strlen(query_buffer));

    if (res) {
        printf("MySQL query error: %s\n",mysql_error(mysql));
        return;
    }

    result = mysql_store_result(mysql);
    if (result)
    {
        dump_field(mysql,&result);
        dump_result(mysql,&result);
    }
    else
        printf("result is NULL\n");

    mysql_free_result(result);
    return;

}

void add(MYSQL* mysql)
{
    // 接收参数
    char para[BUFFER_SIZE];
    scanf("%s", para);
    setbuf(stdin, NULL);
    if (strcmp(para, "-r") * strcmp(para, "--rule") == 0)
    {
        addRule(mysql);
        return;
    }
    if (strcmp(para, "-p") * strcmp(para, "--program") == 0)
    {
        addProgram(mysql);
        return;
    }

    printf("Unrecognized command!\n");
    return;
}

void addRule(MYSQL* mysql)
{
    int id, action_i;
    char auth;

    // 输入程序id
    printf("Program id > ");
    scanf("%d", &id);
    setbuf(stdin, NULL);

    // 输入动作索引
    action_dump();
    printf("Action index > ");
    scanf("%d", &action_i);
    setbuf(stdin, NULL);
    if (switchAction(action_i) == NULL)
        return;

    // 输入权限
    printf("\nAuthority (Y/N)> ");
    auth = tolower(getchar());
    setbuf(stdin, NULL);
    if (auth != 'y' && auth != 'n')
    {
        fprintf(stderr, "\'%c\' is not an available authority token\n", auth);
        return;
    }

    char query_buf[BUFFER_SIZE];

    // 向mysql发送指令
    sprintf(query_buf,"INSERT INTO policy VALUES (0,%d,\"%s\",\"%c\")",id, switchAction(action_i), auth);

    op_handle(mysql,query_buf);
}

void addProgram(MYSQL* mysql)
{
    char name[PARA_SIZE], path[PARA_SIZE], query_buf[BUFFER_SIZE];

    // 输入程序名
    printf("Program name > ");
    scanf("%s",name);
    setbuf(stdin, NULL);

    // 输入程序路径
    printf("Program path (e.g. /bin/bash) > ");
    scanf("%s",path);
    setbuf(stdin, NULL);

    // 向mysql发送指令
    sprintf(query_buf,"INSERT INTO program VALUES (0,\"%s\",\"%s\")",name,path);

    op_handle(mysql,query_buf);

}

void update(MYSQL* mysql)
{
    // 接收参数
    char para[BUFFER_SIZE];
    scanf("%s", para);
    setbuf(stdin, NULL);

    if (strcmp(para, "-r") * strcmp(para, "--rule") == 0)
    {
        updateRule(mysql);
        return;
    }
    if (strcmp(para, "-p") * strcmp(para, "--program") == 0)
    {
        updateProgram(mysql);
        return;
    }

    printf("Unrecognized command!\n");
    return;

}

void updateRule(MYSQL* mysql)
{
    char query_buf[BUFFER_SIZE];
    int id, action_i;
    char auth;
    bool flag;

    // 输入规则id
    printf("Enter the rule's id > ");
    scanf("%d", &id);
    setbuf(stdin, NULL);

    sprintf(query_buf,"SELECT id FROM policy where id=%d",id);
    flag = query_is_null(mysql, query_buf);

    if (flag == ERROR)
        return;

    if (flag)
    {
        printf("Rule id not exists\n");
        return;
    }

    // 输入动作索引
    action_dump();
    printf("Program's new action > ");
    scanf("%d", &action_i);
    setbuf(stdin, NULL);
    if (switchAction(action_i) == NULL)
        return;

    // 输入权限
    printf("\nProgram's new authority (Y/N)> ");
    auth = tolower(getchar());
    setbuf(stdin, NULL);
    if (auth != 'y' && auth != 'n')
    {
        fprintf(stderr, "\'%c\' is not an available authority token\n", auth);
        return;
    }

    sprintf(query_buf,"UPDATE policy SET action=\"%s\",auth=\"%c\" WHERE id=%d", switchAction(action_i),auth,id);
    op_handle(mysql,query_buf);
}

void updateProgram(MYSQL* mysql)
{
    char name[PARA_SIZE], path[PARA_SIZE], query_buf[BUFFER_SIZE];
    int id;
    bool flag;

    // 输入程序id
    printf("Enter the program's id > ");
    scanf("%d", &id);
    setbuf(stdin, NULL);

    sprintf(query_buf,"SELECT id FROM program where id=%d",id);
    flag = query_is_null(mysql, query_buf);

    if (flag == ERROR)
        return;

    if (flag)
    {
        printf("Rule id not exists\n");
        return;
    }

    // 输入程序名
    printf("Program's new name > ");
    scanf("%s", name);
    setbuf(stdin, NULL);

    // 输入程序路径
    printf("Program's new path (e.g. /bin/bash) > ");
    scanf("%s", path);
    setbuf(stdin, NULL);

    // 向mysql发送指令
    sprintf(query_buf,"UPDATE program SET name=\"%s\",path=\"%s\" WHERE id=%d", name, path, id);

    op_handle(mysql,query_buf);
}

void unknown_cmd()
{
    printf("\nUnrecognized command!\n\n");
}