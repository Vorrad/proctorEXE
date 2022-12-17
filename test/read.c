#include <stdio.h>
#include <stdlib.h>


int main(){
    char buf[10000];
    char filename1[100] = "./test1";
    FILE *pf = fopen(filename1, "r");
    if (pf == NULL)
    {
        perror("open test1 failed");
    }
    else
    {
        
        if (fscanf(pf, "%s", buf) < 0)
        {
            perror("read test1 failed");
        }
        else
        {
            printf("read test1 succeed\n");
        }
        fclose(pf);
        
    }

    return 0;
    
}