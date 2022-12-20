#include <stdio.h>
#include <stdlib.h>


int main(){
    char buf[10] = "hello\n";
    char filename1[100] = "./test1";
    FILE *pf = fopen(filename1, "a");
    if (pf == NULL)
    {
        perror("open test1 failed");
    }
    else
    {
        
        if (fputs(buf, pf) < 0)
        {
            perror("write test1 failed");
        }
        fclose(pf);
        printf("write test1 succeed\n");
    }

    
    return 0;
    
}
