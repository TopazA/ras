#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

// Define reserved commands
#define CD 		1
#define VIM 	2
#define LLS 	3
#define LCD 	4
#define LMKDIR 	5
#define LRMDIR 	6
#define LRM 	7
#define GET 	8
#define PUT 	9
#define HELP 	10
#define MAIN 	11
#define OTHER 	12
#define EXIT 	13

#define MAX_ENTRIES			64
#define CONF_LINE_LENGTH	64
#define DIR_LENGTH			256
#define COMMAND_LENGTH		256

struct config_structure 
{
	char server[MAX_ENTRIES][CONF_LINE_LENGTH];
	char hostname[MAX_ENTRIES][CONF_LINE_LENGTH];
	char user[MAX_ENTRIES][CONF_LINE_LENGTH];
	char cwd[MAX_ENTRIES][DIR_LENGTH];
	int quantity;
	int selected;
};

typedef struct config_structure config;

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

// Print list of available servers
int print_list_server(config * conf)
{
	int j;
	printf("\n");
	for(j = 1; j <= conf->quantity;j++)
	{
		printf("%d / %s:\t",j,conf->server[j]);
		printf("%s@%s\n",conf->user[j],conf->hostname[j]);
	}
	printf("\n");
	return 1;
}

// Launch an ssh command on the remote selected server
int ssh(config * conf, char * line)
{
	char * command = (char *)malloc(DIR_LENGTH);

	snprintf(command,DIR_LENGTH,"ssh -Y %s@%s 'cd \"%s\" ; %s'",
			conf->user[conf->selected],conf->hostname[conf->selected],conf->cwd[conf->selected],line);
	system(command);

	free(command);
	return 0;
}

// Get the current working directory
int pwd(config * conf)
{
	FILE * fd;
	char * command = (char *)malloc(DIR_LENGTH);

	if(strlen(conf->cwd[conf->selected]) <= 0)
		snprintf(command,DIR_LENGTH,"ssh %s@%s pwd",conf->user[conf->selected],conf->hostname[conf->selected]);

	fd = popen(command,"r");
	fread_line(fd,conf->cwd[conf->selected]);
	pclose(fd);

	free(command);
	return 0;
}

// Edit a file with vim
int vim(config * conf,char * line)
{
	int a;
	int e = 0;
	char * command = (char *) malloc(DIR_LENGTH);
	char * l2 = (char *) malloc(DIR_LENGTH);
	strncpy(l2,line,DIR_LENGTH);

	for(a = 0; a < strlen(l2); a++)
	{
		if(a < 4)
			continue;

		if(l2[a] == ';')
			l2[e] = '\0';

		l2[e] = l2[a];
		e++;
	}
	l2[e] = '\0';

	snprintf(command,DIR_LENGTH,"vim scp://%s@%s/%s/%s"
			,conf->user[conf->selected],conf->hostname[conf->selected],conf->cwd[conf->selected],l2);

	printf("+++ %s +++\n",command);
	system(command);
	free(command);
	free(l2);
	return 0 ;
}

// Change directory for a server
int cd(config * conf,char * line)
{
	int a;
	int j = 0;
	char * command = (char *) malloc(1024);
	FILE * fd;

	for(a = 0; a < strlen(line); a++)
	{
		if(line[a] == ';'
			|| line[a] == '\0')
		{
			line[j] = '\0';
			break;
		}
		line[j++] = line[a];
	}

	if(strlen(conf->cwd[conf->selected]) == 0)
		snprintf(command,1024,"ssh %s@%s '%s ; pwd'",
							conf->user[conf->selected],conf->hostname[conf->selected],line);
	else
		snprintf(command,1024,"ssh %s@%s 'cd \"%s\" ; %s ; pwd'",
							conf->user[conf->selected],
							conf->hostname[conf->selected],conf->cwd[conf->selected],line);

	printf("%s\n",command);
	fd = popen(command,"r");
	fread_line(fd,conf->cwd[conf->selected]);
	pclose(fd);
	printf("--%s--\n",conf->cwd[conf->selected]);

	free(command);
	return 0;
}

int help()
{
	printf("\tRAS Ver 0.03\n\n");
	printf("\t\tTo connect to a server\n");
	printf("\tType the number of the server you want to connect to\n\n");
	printf("\t\tTo disconnect from a server\n");
	printf("\tType 'm', it brings you back to main menu\n\n");
	printf("\tOnce connected to a server, you can use any *non* interactive command\n");
	printf("\tUsage of interactive software is not recommended, result is not warranted\n");
	printf("\tEdit a file using the command 'vim' followed by the name of the file\n");
	printf("\t(Vim is required for this last feature to work, other editor are *NOT* supported\n\n"); 
	printf("\tBug reports to stephane@unices.org\n");
	printf("\tYour configuration file is in %s/.ras/config\n",getenv("HOME"));
	return 0;
}

// return 0 if line start with p
int start_with(char * p,char * line)
{
	int i;
	for(i = 0; i < strlen(p); i++)
		if(p[i] != line[i])
			return -1;
	return 0;
}

//Get the command type
int get_command_type(char * line)
{
	int type;

	if(line == NULL || strcmp(line,"exit") == 0)
		type = EXIT;

	else if(start_with("cd ",line) == 0)
		type = CD;

	else if(start_with("vim ",line) == 0)
		type = VIM;
	
	else if(strcmp(line,"help") == 0 || strcmp(line,"?") == 0)
		type = HELP;

	else if(strcmp(line,"m") == 0)
		type = MAIN;

	else if(start_with("get ",line) == 0)
		type = GET;
	
	else if(start_with("put ",line) == 0)
		type = PUT;
	
	else type = OTHER;
	
	return type;
}

// Main ;)
int main (int argc, char * argv[])
{
	config conf;
	conf.selected = 0;
	char * prompt = (char *)malloc(256);
	int i;
	char * input;
	int type;

	snprintf(prompt,256," ==> ");
	create_default_config_file();
	i = load_config_file(&conf);
	print_list_server(&conf);
	while(1)
	{
		input = readline(prompt);
		type = get_command_type(input);

		switch(type)
		{
			case EXIT:
				printf("Bye\n");
				free(prompt);
				return 0;

			case HELP:
				help();
				break;

			case MAIN:
				if(conf.selected != 0)
				{
					snprintf(prompt,256," ==> ");
					conf.selected = 0;
					break;
				}
			
			case CD:
				if(conf.selected != 0)
				{
					cd(&conf,input);
					snprintf(prompt,256,"[ %s ] %s@%s:%s ==> ",
								conf.server[conf.selected],conf.user[conf.selected],
								conf.hostname[conf.selected],conf.cwd[conf.selected]);
					break;
				}

			case VIM:
				if(conf.selected != 0 )
				{
			 		vim(&conf,input);
					break;
				}

			case OTHER:
				if(conf.selected == 0 && atoi(input) <= i && atoi(input) >= 1)
				{
					conf.selected = atoi(input);
					pwd(&conf);
					snprintf(prompt,256,"[ %s ] %s@%s:%s ==> ",
								conf.server[conf.selected],conf.user[conf.selected],
								conf.hostname[conf.selected],conf.cwd[conf.selected]);
					break;
				}
				else if(conf.selected != 0 && strlen(input) > 1)
				{
					ssh(&conf,input);
					break;
				}

				else if(conf.selected == 0)
				{
					print_list_server(&conf);
					break;
				}
			default:
				break;
		}
		add_history(input);
		free(input);
	}
	free(prompt);
	return 0;
}
