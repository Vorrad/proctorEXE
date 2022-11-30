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
    char read_buf[BUFFER_SIZE];
    FILE* fp;

    if ((fp = fopen(ACTION_LIST, "r")) == NULL)
    {
        printf("actions: fail to open file.\n");
        return;
    }

    // 输入程序id
    printf("Program id > ");
    scanf("%d", &id);
    setbuf(stdin, NULL);

    // 输入动作索引
    printf("\nAction list:\n\n");
    while (fgets(read_buf, BUFFER_SIZE, fp) != NULL)
        printf("%s",read_buf);
    
    printf("Action index > ");
    scanf("%d", &action_i);
    setbuf(stdin, NULL);
    if (switchAction(action_i) == NULL)
        return;

    // 输入权限索引
    printf("\nAuthority (Y/N)> ");
    auth = getchar();
    setbuf(stdin, NULL);
    auth = tolower(auth);
    if (auth != 'y' && auth != 'n')
    {
        fprintf(stderr, "\'%c\' is not an available authority token\n", auth);
        return;
    }
    
    char query_buffer[BUFFER_SIZE], action_buffer[BUFFER_SIZE];
    int res;    

    // 向mysql发送指令
    sprintf(query_buffer,"INSERT INTO policy VALUES (0,%d,\"%s\",\"%c\")",id, switchAction(action_i), auth);
    // printf("%s\n",query_buffer);
    res = mysql_real_query(mysql, query_buffer, (u_long) strlen(query_buffer));
    
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

void addProgram(MYSQL* mysql)
{

}

void unknown_cmd()
{
    printf("\nUnrecognized command!\n\n");
}