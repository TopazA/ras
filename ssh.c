#include <libssh/libssh.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>



int verify_knownhost(ssh_session session);
int verify_local_knownhost(char * host,ssh_session ps);
int save_local_host(char * host, char * key);

int main(int argc,char * argv[])
{
	ssh_session ps;
	int rc;
	char * host = argv[1];

	ps = ssh_new();
	ssh_options_set(ps,SSH_OPTIONS_HOST,host);

	rc = ssh_connect(ps);

    if (rc != SSH_OK)
	{
		fprintf(stderr, "Error connecting to localhost: %s\n", ssh_get_error(ps));
		return -1;
	}
	if((rc = verify_local_knownhost(host,ps)) != 0)
		return -1;

    ssh_disconnect(ps);
	ssh_free(ps);

	return 0;
}

int verify_local_knownhost(char * host,ssh_session ps)
{
	unsigned char * key = (unsigned char *) malloc(1024);
	int hlen;
	char * hkey;

	hlen = ssh_get_pubkey_hash(ps, &key);

	if (hlen < 0)
		return -1;

	hkey = ssh_get_hexa	(key, hlen);

	save_local_host(host,hkey);
	
	printf("--\n%s\n--",hkey);

	free(key);
	free(hkey);
	return 0;
}

int save_local_host(char * host, char * key)
{
	char * dir = (char *) malloc(256);
	char * file = (char *) malloc(256);
	DIR * pdir;
	FILE * fd;

	snprintf(dir,(size_t)256,"%s/.ras/ssh",getenv("HOME"));

	if((pdir = opendir(dir)) != NULL)
		closedir(pdir);
	else
		mkdir(dir,0700);
	
	snprintf(file,(size_t)256,"%s/%s",dir,host);

	if((fd = fopen(file,"w")) == NULL)
	{
		fprintf(stderr,"Can not open %s\n",file);
		free(file);
		free(dir);
		return -1;
	}

	fprintf(fd,"%s\n",key);
	fclose(fd);

	free(file);
	free(dir);
	return 0;
}

	

