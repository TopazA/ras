
#include "lcommand.h"

int clean_line(char * line)
{
	int i;
	for(i = 0; i < strlen(line); i++)
		line[i] = line[i+1];
	return i;
}

int extract_dir(char * line)
{
	int i, j = 0;
	int cond = 0;

	for (i = 0; i < strlen(line); i++)
	{
		if(i > 3 && line[i] != ' ')
			cond = 1;

		if(cond == 1)
			line[j++] = line[i];
	}
	line[j] = '\0';
	return i;

}

int lpwd()
{
	char * wd = (char *) malloc(1024);
	getcwd(wd,1024);
	printf("Current working directory: %s\n",wd);
	free(wd);
	return 1;
}

int lcd(char * line)
{
	extract_dir(line);
	return chdir(line);
}

int lls(char * line)
{
	FILE * fd;
	char c;
	clean_line(line);
	if((fd = popen(line,"r")) != NULL)
	{
		while((c = fgetc(fd)) != EOF)
		{
			fputc(c,stdout);
		}
		pclose(fd);
	}
	else
	{
		fprintf(stderr,"Can not execute command: %s\n",line);
		return 0;
	}
	return 1;
}



