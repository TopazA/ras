#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>


#define SERVER 	"192.168.0.170"
#define USER	"stephane"
int verify_knownhost(ssh_session session);
int verify_local_knownhost(ssh_session ps);

int main(int argc,char * argv[])
{
	ssh_session ps;
	int rc;

	ps = ssh_new();
	ssh_options_set(ps,SSH_OPTIONS_HOST,SERVER);

	rc = ssh_connect(ps);

    if (rc != SSH_OK)
	{
		fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(ps));
		return -1;
	}
	if((rc = verify_local_knownhost(ps)) != 0)
		return -1;

    ssh_disconnect(ps);
	ssh_free(ps);

	return 0;
}

int verify_local_knownhost(ssh_session ps)
{
	unsigned char * key = (unsigned char *) malloc(1024);
	int hlen;
	char * hkey;

	hlen = ssh_get_pubkey_hash(ps, &key);

	if (hlen < 0)
		return -1;

	hkey = ssh_get_hexa	(key, hlen);
	
	printf("--\n%s\n--",hkey);

	free(key);
	free(hkey);
	return 0;
}

int save_local_host(char * host, char * key)
{
	char * dir = (char *) malloc(256);
	DIR * pdir;

	dir = snprintf(dir,256,"%s/.ras/ssh/",get_env("HOME"));

	if((pdir = opendir(dir)) != NULL)
		closedir(pdir);
	else
		mkdir(dir,0700);
	
}

	

