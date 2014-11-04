RAS
===

RAS ('Rien à Signaler' in French) is a remote system administration 
tool over SSH.

RAS is nothing more than a wrapper to call some ssh command from a
remote server. For each command, RAS will establish a SSH connection,
launch a command and return its result. It looks dirty to establish a
new SSH connection, however if you hate being disconnected permanently
from your SSH connection because of a dumb 3G connection, if you hate
typing on your keyboard and see that it needs 2 secondes for the letters
to display on your screen, then RAS is for you. 

With RAS, you always work in local. Is it useful ? It is, because I love
using it when my Internet connection sucks and I have to work with a
remote server.

It can as well 'get' and 'put' files and run local commands (ssh does
not know how to do that).

To edit a file, RAS is using vim and vim only. If you like Emacs, RAS is
not a tool for you (anyway, if you use Emacs, you don't need any other
tool than Emacs). Just type 'vim [file]' and it will launch a local
instance of vim with remote content of the file.

Aliases commands are supported.

First time you start RAS, it will create minimal configuration file in
~/.ras/config. Edit this file and add your own servers. Then restart
RAS, a menu will display available servers with user. Just hit it's
number and you're connected, you can now use any Unix command.

To install it, just compile it with 'make' and copy the binary file
where ever fits you. Launch it and type 'help' to get more information.

You have a question, you think RAS is a great software and you want some
features, you notices a bug or you just want to say 'Hi'. Send me an
email to stephane@unices.org

BUGS: At the end of each command sent, you'll see a line like this one :

Connection to 127.0.0.1 closed

This is just the ssh client displaying that he closed the connection
with the server. It does not mean anything for you, RAS has no permanent
connection with the server, it will establish a new connection for each
command. Therefor, you can ignore this warning.
