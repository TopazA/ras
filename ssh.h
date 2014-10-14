#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define SSHKEY_OK 		0
#define SSHKEY_ABSENT 	1
#define SSHKEY_DIFFER 	2
#define SSHKEY_ERROR 	3


