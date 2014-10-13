#include "config.h"

// Create a default configuration file in $HOME/.ras/config if none exists.
int create_default_config_file()
{
	DIR * dir;
	FILE * fd;
	char * config_dir = (char *) malloc(DIR_LENGTH);
	char * config_file = (char *) malloc(DIR_LENGTH);
	char * user = getenv("USER");

	snprintf(config_dir,DIR_LENGTH,"%s/.ras", getenv("HOME"));
	if((dir = opendir(config_dir)) != NULL)
		closedir(dir);
	else
		mkdir(config_dir,0700);

	snprintf(config_file,DIR_LENGTH,"%s/config",config_dir);
	if((fd = fopen(config_file,"r")) != NULL)
		fclose(fd);
	else
	{
		if(( fd = fopen(config_file,"w")) != NULL)
		{
			fprintf(fd,"[localhost]\n\n");
			fprintf(fd,"users=%s\n",user);
			fprintf(fd,"hostname=127.0.0.1\n");
			fclose(fd);
		}
		else
		{
			fprintf(stderr,"FATAL: Can not open %s for writing\n",config_file);
			exit(-1);
		}
	}

	free(config_file);
	free(config_dir);
	return 0;
}

//Returns the next line for a opened FD. '\n' is already cleaned up.
int fread_line(FILE * fd,char * line)
{
	char a;
	int i = 0;
	while((a = fgetc(fd)) != EOF)
	{
		if(a == '\n')
		{
			line[i] = '\0';
			return i;
		}
		line[i++] = a;
	}
	return EOF;
}

// Detects and clean a section (starting with '['), returns 1 if it's a section, 0 if not
int clean_section(char * text)
{
	int i;
	if(text[0] != '[')
		return 0;
	for(i = 0; i <= strlen(text); i++)
	{
		if(text[i+1] == ']')
		{
			text[i] = '\0';
			return 1;
		}
		text[i] = text[i+1];
	}
	return 1;
}

// Get configuration value, returns 1 for an hostname and 2 for an user, 0 for anything else
int get_value(char * text)
{
	int i;
	int j = 0;
	int returned_value = 0;
	char * t1 = (char *)malloc(256);
	char * t2 = (char *)malloc(256);
	int cond = 0;

	for(i = 0; i <= strlen(text); i++)
	{
		if(text[i] == '=' && cond == 0)
		{
			t1[j] = '\0';
			cond = 1;
			j = 0;
			continue;
		}
		else if(cond == 0)
			t1[j] = text[i];
		else
			t2[j] = text[i];
		j++;
	}
	if(strcmp(t1,"users") == 0 || strcmp(t1,"user") == 0)
		returned_value = 2;
	if(strcmp(t1,"hostname") == 0)
		returned_value = 1;
	strncpy(text,t2,1024);

	free(t1);
	free(t2);

	return returned_value;

}

// Load the configuration file
int load_config_file(config * conf)
{
	char * config_file = (char *) malloc(DIR_LENGTH);
	FILE * fd;
	char * line = (char *) malloc(1024);
	int i = 0;
	int type;
	int x = 0;

	snprintf(config_file,DIR_LENGTH,"%s/.ras/config",getenv("HOME"));
	if((fd = fopen(config_file,"r")) == NULL)
	{
		fprintf(stderr,"FATAL: Can not open %s for reading\n",config_file);
		exit(-1);
	}

	while(fread_line(fd,line) != EOF)
	{
		x++;
		if(line[0] == '\0')
			continue;
		if(clean_section(line) == 1)
		{
			i++;
			strncpy(conf->server[i],line,64);
		}
		else
		{
			type = get_value(line);
			if(type == 0)
				fprintf(stderr,"WARNING error in config file line %d\n",x);
			if(type == 1)
				strncpy(conf->hostname[i],line,64);
			if(type == 2)
				strncpy(conf->user[i],line,64);
		}
	}
	fclose(fd);

	free(config_file);
	free(line);
	conf->quantity = i;
	return i;
}

