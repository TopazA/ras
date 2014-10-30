
#include "alias.h"

	   /* Create a new hashtable. */
hashtable_t * ht_create (int size)
{
	hashtable_t *hashtable = NULL;
	int i;

	if (size < 1)
		return NULL;

  /* Allocate the table itself. */
	if ((hashtable = malloc (sizeof (hashtable_t))) == NULL)
    {
		return NULL;
    }

  /* Allocate pointers to the head nodes. */
	if ((hashtable->table = malloc (sizeof (entry_t *) * size)) == NULL)
   	{
		return NULL;
    }
  	for (i = 0; i < size; i++)
    {
      hashtable->table[i] = NULL;
    }

  hashtable->size = size;

  return hashtable;
}

		/* Hash a string for a particular hash table. */
int ht_hash (hashtable_t * hashtable, char *key)
{

  unsigned long int hashval;
  int i = 0;

  /* Convert our string to an integer */
  while (hashval < ULONG_MAX && i < strlen (key))
    {
      hashval = hashval << 8;
      hashval += key[i];
      i++;
    }

  return hashval % hashtable->size;
}

		 /* Create a key-value pair. */
entry_t * ht_newpair (char *key, char *value)
{
  entry_t *newpair;

  if ((newpair = malloc (sizeof (entry_t))) == NULL)
    {
      return NULL;
    }

  if ((newpair->key = strdup (key)) == NULL)
    {
      return NULL;
    }

  if ((newpair->value = strdup (value)) == NULL)
    {
      return NULL;
    }

  newpair->next = NULL;

  return newpair;
}

		  /* Insert a key-value pair into a hash table. */
void ht_set (hashtable_t * hashtable, char *key, char *value)
{
  int bin = 0;
  entry_t *newpair = NULL;
  entry_t *next = NULL;
  entry_t *last = NULL;

  bin = ht_hash (hashtable, key);

  next = hashtable->table[bin];

  while (next != NULL && next->key != NULL && strcmp (key, next->key) > 0)
    {
      last = next;
      next = next->next;
    }

  /* There's already a pair.  Let's replace that string. */
  if (next != NULL && next->key != NULL && strcmp (key, next->key) == 0)
    {

      free (next->value);
      next->value = strdup (value);

      /* Nope, could't find it.  Time to grow a pair. */
    }
  else
    {
      newpair = ht_newpair (key, value);

      /* We're at the start of the linked list in this bin. */
      if (next == hashtable->table[bin])
	{
	  newpair->next = next;
	  hashtable->table[bin] = newpair;

	  /* We're at
	   * the end
	   * of the
	   * linked
	   * list in
	   * this bin.
	   * */
	}
      else if (next == NULL)
	{
	  last->next = newpair;

	  /* We're
	   * in
	   * the
	   * middle
	   * of
	   * the
	   * list.
	   * */
	}
      else
	{
	  newpair->next = next;
	  last->next = newpair;
	}
    }
}

		   /* Retrieve a key-value pair from a hash table. */
char * ht_get (hashtable_t * hashtable, char *key)
{
  int bin = 0;
  entry_t *pair;

  bin = ht_hash (hashtable, key);

  /* Step through the bin, looking for our value. */
  pair = hashtable->table[bin];
  while (pair != NULL && pair->key != NULL && strcmp (key, pair->key) > 0)
    {
      pair = pair->next;
    }

  /* Did we actually find anything? */
  if (pair == NULL || pair->key == NULL || strcmp (key, pair->key) != 0)
    {
      return NULL;

    }
  else
    {
      return pair->value;
    }

}

//Returns the next line for a opened FD. '\n' is already cleaned up.
int fread_alias(FILE * fd,char * line)
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


// Get configuration value, returns 1 for an hostname and 2 for an user, 0 for anything else
int save_alias(hashtable_t * alias,char * text)
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
	if(strlen(t1) > 0 && strlen(t2) > 0)
	{
		fprintf(stderr,"%s = %s\n",t1,t2);
		ht_set(alias,t1,t2);
		returned_value = 1;
	}

	free(t1);
	free(t2);

	return returned_value;

}

int load_alias(hashtable_t * alias)
{
	char * home = getenv("HOME");
	char * alias_file = (char *) malloc(1024);
	FILE * fd;
	int i = 0;
	char * line = (char *) malloc(1024);
	snprintf(alias_file,1024,"%s/.ras/alias",home);
	if((fd = fopen(alias_file,"r")) != NULL)
	{
		while(fread_alias(fd,line) != EOF)
			i = i + save_alias(alias,line);

		fclose(fd);
	}
	free(alias_file);
	free(line);
	return i;
}

int get_alias(hashtable_t * alias,char * command)
{
	char * arg0 = (char *)malloc(256);
	char * after = (char *)malloc(256);
	int i;
	for(i = 0; i < strlen(command);i++)
	{
		if(command[i] == ' ')
			break;
		arg0[i] = command[i];
	}
	arg0[i] = '\0';
	after = ht_get(alias,arg0);
	if(strlen(after) > 0)
		fprintf(stderr,"Found for '%s' the alias '%s'\n",command,after);

	return 0;
}


/*
int
main (int argc, char **argv)
{

  hashtable_t *hashtable = ht_create (65536);

  ht_set (hashtable, "key1", "inky");
  ht_set (hashtable, "key2", "pinky");
  ht_set (hashtable, "key3", "blinky");
  ht_set (hashtable, "key4", "floyd");

  printf ("%s\n", ht_get (hashtable, "key1"));
  printf ("%s\n", ht_get (hashtable, "key2"));
  printf ("%s\n", ht_get (hashtable, "key3"));
  printf ("%s\n", ht_get (hashtable, "key4"));

  return 0;
}
*/
