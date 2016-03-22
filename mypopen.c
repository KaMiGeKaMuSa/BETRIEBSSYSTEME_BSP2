#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *fp;
    
    CheckArguments(argc, argv);
    
    if ((fp = popen("ls -rt", "r")) != NULL))
    {
        ProcessOutput(fp);
        
        (void) pclose(fp);
        
    }
  
    exit(EXIT_SUCCESS);
    
}
