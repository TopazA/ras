#include "ssh.h"


int verify_knownhost(ssh_session session);
int verify_local_knownhost(char * host,ssh_session ps);
int compare_key(char * host, char * key);
int fread_line(FILE * fd,char * line);

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
	int check_key;
	int r;

	hlen = ssh_get_pubkey_hash(ps, &key);

	if (hlen < 0)
		return -1;

	hkey = ssh_get_hexa	(key, hlen);

	check_key = compare_key(host,hkey);

	switch(check_key)
	{
		case SSHKEY_OK:
			r = 0;
			break;
		
		case SSHKEY_DIFFER:
			fprintf(stderr,"Error: Server public key differs\n");
			r = -1;
			break;

		case SSHKEY_ABSENT:
			fprintf(stderr,"Info: Public key unknown, saved as valid key for this server\n");
			r = 0;
			break;

		default:
			fprintf(stderr,"Error: Unknown error\n");
			r = -1;
	}
	
	printf("--\n%s\n--",hkey);

	free(key);
	free(hkey);
	return r;
}

int compare_key(char * host, char * key)
{
	char * dir = (char *) malloc(256);
	char * file = (char *) malloc(256);
	char * line = (char *) malloc(256);
	DIR * pdir;
	FILE * fd;
	int r;

	snprintf(dir,(size_t)256,"%s/.ras/ssh",getenv("HOME"));

	if((pdir = opendir(dir)) != NULL)
		closedir(pdir);
	else
		mkdir(dir,0700);
	
	snprintf(file,(size_t)256,"%s/%s",dir,host);

	if((fd = fopen(file,"r")) == NULL)
	{
		if((fd = fopen(file,"w")) == NULL)
		{
			fprintf(stderr,"Can not open %s\n",file);

			r = SSHKEY_ERROR;
		}
		else
		{
			fprintf(fd,"%s\n",key);
			fclose(fd);

			r = SSHKEY_ABSENT;
		}
	}
	else
	{
		fread_line(fd,line);
	
		if(strcmp(line,key) != 0)
			r = SSHKEY_DIFFER;
		else
			r = SSHKEY_OK;

		fclose(fd);
	}

	free(file);
	free(dir);
	free(line);
	return r;
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


	

