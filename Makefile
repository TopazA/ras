


ALL:
	cc -o ras ras.c config.c -Wall -lreadline -lcurses
	cc -o ssh2 -Wall ssh2.c -l ssh2
# Let's forget libssh for now, it's not complete
#	cc -o ssh -Wall ssh.c -l ssh
