
#include "ras.h"

char * extract_file(char * line)
{
	int i;

	for(i = 0; i < strlen(line); i++)
	{
		if(line[0] == ' ' && i > 2)
			break;
		line++;
	}
	line++;
	return(line);
}


int get(config * conf, char * line)
{
	char * command = (char *)malloc(256);

	snprintf(command,256,"scp %s@%s:%s/%s ./",
			conf->user[conf->selected],conf->hostname[conf->selected],conf->cwd[conf->selected],extract_file(line));

	fprintf(stderr,"%s\n",command);
	system(command);

	free(command);
	return 0;
}


int put(config * conf, char * line)
{
	char * command = (char *)malloc(256);

	snprintf(command,256,"scp %s %s@%s:%s/",
			extract_file(line),conf->user[conf->selected],conf->hostname[conf->selected],conf->cwd[conf->selected]);

	fprintf(stderr,"%s\n",command);
	system(command);

	free(command);
	return 0;
}

// Print list of available servers
int print_list_server(config * conf)
{
	int j;
	printf("\n");

	for(j = 1; j <= conf->quantity;j++)
		printf("%d / %s:\t%s@%s\n",j,conf->server[j],conf->user[j],conf->hostname[j]);

	printf("\n");

	return 1;
}

// Launch an ssh command on the remote selected server
int ssh(config * conf, char * line)
{
	char * command = (char *)malloc(DIR_LENGTH);

	snprintf(command,DIR_LENGTH,"ssh -t -Y %s@%s 'cd \"%s\" ; %s'",
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

	if((fd = popen(command,"r")) == NULL)
	{
		fprintf(stderr,"FATAL: %s\n", strerror(errno));
		exit(-1);
	}
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

	snprintf(command,DIR_LENGTH,"vim scp://%s@%s/%s/%s",
		conf->user[conf->selected],conf->hostname[conf->selected],conf->cwd[conf->selected],l2);

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

	fd = popen(command,"r");
	fread_line(fd,conf->cwd[conf->selected]);
	pclose(fd);

	free(command);
	return 0;
}

int help()
{
	printf("\tRAS Ver 0.05a\n\n");
	printf("\t\tTo connect to a server\n");
	printf("\tType the number of the server you want to connect to\n\n");
	printf("\t\tTo disconnect from a server\n");
	printf("\tType 'm', it brings you back to main menu\n\n");
	printf("\tOnce connected to a server, you can use any command\n");
	printf("\tEdit a file using the command 'vim' followed by the name of the file\n");
	printf("\t(Vim is required for this last feature to work, other editor are *NOT* supported\n\n"); 
	printf("\tLocal commands lls, lmkdir, lrmdir, lcd and lpwd are available\n\n");
	printf("\tget and put allow you to transfer files\n\n");
	printf("\tYour configuration file is in %s/.ras/config\n",getenv("HOME"));
	printf("\tBy typing 'edit_config' from ras, you can edit the configuration file\n\n");
	printf("\tYou can define some alias in %s/.ras/alias\n",getenv("HOME"));
	printf("\tls=ls -l will replace 'ls' by 'ls -la'\n\n");
	printf("\n\tBug reports to stephane@unices.org\n");
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
	
	else if(strcmp(line,"edit_config") == 0)
		type = EDIT_CONFIG;
	
	else if(start_with("lls ",line) == 0
			|| strcmp(line,"lls") == 0
			|| start_with("lrmdir ",line) == 0
			|| start_with("lmkdir ",line) == 0)
		type = LLS;
	
	else if(start_with("lcd ",line) == 0)
		type = LCD;
	
	else if(strcmp(line,"lpwd") == 0)
		type = LPWD;

	else type = OTHER;
	
	return type;
}

// Edit Config
int edit_config(config * conf)
{
	char * editor = getenv("EDITOR");
	char * cmd = (char *)malloc(256);

	if(editor == NULL)
		snprintf(cmd,256,"vi %s/.ras/config",getenv("HOME"));
	else
		snprintf(cmd,256,"%s %s/.ras/config",editor,getenv("HOME"));

	system(cmd);
	free(cmd);

	clean_conf(conf);
	return load_config_file(conf);
}

// Main ;)
int main (int argc, char * argv[])
{
	config conf;
	conf.selected = 0;
	char * prompt = (char *)malloc(PROMPT_SIZE);
	int i;
	char * input;
	int type;
	char * rinput = (char *) malloc(1024);
  	hashtable_t * db_alias = ht_create (65536) ;

	snprintf(prompt,PROMPT_SIZE," ==> ");
	create_default_config_file();
	i = load_config_file(&conf);
	load_alias(db_alias);
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

			case EDIT_CONFIG:
				i = edit_config(&conf);
				snprintf(prompt,256," ==> ");
				break;

			case HELP:
				help();
				break;

			case LLS:
				lls(input);
				break;

			case LCD:
				if(lcd(input) != 0)
					fprintf(stderr,"Error changing directory\n");
				break;

			case LPWD:
				lpwd();
				break;

			case MAIN:
				if(conf.selected != 0)
				{
					snprintf(prompt,256," ==> ");
					conf.selected = 0;
				}
				break;
			
			case CD:
				if(conf.selected != 0)
				{
					cd(&conf,input);
					snprintf(prompt,PROMPT_SIZE,"[ %s ] %s@%s:%s ==> ",
								conf.server[conf.selected],conf.user[conf.selected],
								conf.hostname[conf.selected],conf.cwd[conf.selected]);
				}
				break;

			case VIM:
				if(conf.selected != 0 )
			 		vim(&conf,input);
				break;

			case GET:
				if(conf.selected != 0)
					get(&conf,input);
				break;

			case PUT:
				if(conf.selected != 0)
					put(&conf,input);
				break;

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
				else if(conf.selected != 0 && strlen(input) > 0)
				{
					get_alias(db_alias,input,rinput);
					if(strlen(rinput) == 0)
						ssh(&conf,input);
					else
						ssh(&conf,rinput);
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

