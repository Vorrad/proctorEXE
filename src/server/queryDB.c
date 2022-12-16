#include "../../include/server/queryDB.h"

int ret_auth(pid_t pid)
{
    char proc_path[PATH_SIZE];

    sprintf(proc_path, "/proc/%d/", pid);

    printf("proc path: %s\n", proc_path);

    return 1;
}