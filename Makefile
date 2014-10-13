


ALL:
	cc -o ras ras.c config.c -Wall -lreadline -lcurses
	cc -o ssh -Wall ssh.c -l ssh
