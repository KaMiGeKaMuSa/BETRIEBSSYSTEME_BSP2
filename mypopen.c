#include <stdio.h>
    
FILE *mypopen(const char *command, const char *type)
{
	return popen(command, type);
}
	  
int mypclose(FILE *stream)
{
	return pclose(stream);
}