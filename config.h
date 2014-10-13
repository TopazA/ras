#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

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
int create_default_config_file();

//Returns the next line for a opened FD. '\n' is already cleaned up.
int fread_line(FILE * fd,char * line);

// Load the configuration file
int load_config_file(config * conf);

