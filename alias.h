
#include <stdio.h>

#define _XOPEN_SOURCE 500	/* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct entry_s
{
	char *key;
	char *value;
	struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s
{
	int size;
	struct entry_s **table;
};

typedef struct hashtable_s hashtable_t;



