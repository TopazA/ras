#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "config.h"
#include "lcommand.h"
#include "alias.h"

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
#define EDIT_CONFIG 14
#define LPWD	15

#define PROMPT_SIZE 256



