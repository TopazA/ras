
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

hashtable_t * ht_create (int size);
void ht_set (hashtable_t * hashtable, char *key, char *value);
char * ht_get (hashtable_t * hashtable, char *key);
int load_alias(hashtable_t * alias);




