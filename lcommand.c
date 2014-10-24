
#include "lcommand.h"

int lls(char * line)
{
	FILE * fd;
	char c;
	if((fd = popen("ls -l","r")) != NULL)
	{
		while((c = fgetc(fd)) != EOF)
		{
			fputc(c,stdout);
		}
		pclose(fd);
	}
	return 1;
}


