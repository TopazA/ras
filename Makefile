
clean:
	rm ras


ALL:
	cc -o ras ras.c -Wall -lreadline -lcurses
