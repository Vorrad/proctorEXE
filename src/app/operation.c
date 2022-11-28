#include "../../include/app/operation.h"

void help()
{
printf(

"\ncommand\t\t\tdescription\n\n\
help / h\t\t\tshow this help info.\n\
quit / q\t\t\tquit the control panel.\n\
exit / e\t\t\tsame as \'quit\'.\n\
show / s\t\t\tshow current rules.\n\n\
add  / a path\t\t\t\
\n\n");
}

void showRules(MYSQL* mysql)
{
    int res;
    char query_buffer[BUFFER_SIZE];
    strcpy(query_buffer,"SELECT * FROM ");
    strcat(query_buffer, TABLE_POLICY);
    printf("querybuf: %s\n",query_buffer);
    res = mysql_real_query(mysql, query_buffer, (u_long) strlen(query_buffer));
    
    if (res) {
        printf("MySQL 查询出错: %s\n",mysql_error(mysql));
        return;
    }

    MYSQL_RES* result;
    
    result = mysql_store_result(mysql);
    printf("查询成功\n");
    
    if (result)
    {
        dump_field(mysql,&result);
        dump_result(mysql,&result);
    }
    else
        printf("result is NULL\n");
    


    printf("查询成功\n");

    mysql_free_result(result);

}

void unknown_cmd()
{
    printf("无法识别您的指令，请输入 help 查看指令格式\n");
}

void control_panel(MYSQL* mysql)
{
    char command[BUFFER_SIZE], para[BUFFER_SIZE];
    
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
            showRules(mysql);
            continue;
        }
        unknown_cmd();    
    }
        
}