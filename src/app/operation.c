#include "../../include/app/operation.h"

void help()
{
    FILE* fp = NULL;
    char read_buf[BUFFER_SIZE];

    if ((fp = fopen(HELP_FILE, "r")) == NULL)
    {
        printf("help: fail to open file.");
        return;
    }

    printf("\n\n");
    while (fgets(read_buf, BUFFER_SIZE, fp) != NULL)
        printf("%s",read_buf);
    printf("\n\n");
    
    fclose(fp);
}

void unknown_cmd()
{
    printf("Unrecognized command!\n");
}

void control_panel(MYSQL* mysql)
{
    char command[BUFFER_SIZE];
    
    while (true)
    {
        printf("Please enter command(enter 'help' for help)> ");    
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
        unknown_cmd();
    }
        
}

void show(MYSQL* mysql)
{
    // 接收参数
    char para[BUFFER_SIZE];
    scanf("%s", para);
    fflush(stdin);

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