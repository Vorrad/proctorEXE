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
    
    if (result)
    {
        dump_field(mysql,&result);
        dump_result(mysql,&result);
    }
    else
        printf("result is NULL\n");

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