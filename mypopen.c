#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *fp;
    extern FILE *popen();
    char buff[512];
 
    if(!(fp = popen("ls -sail", "r"))){
        exit(1);
    }

    while(fgets(buff, sizeof(buff), fp)!=NULL){
        printf("%s", buff);
    }
    pclose(fp);

    
}
