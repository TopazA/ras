ras
===

ras (Rien à Signaler in French) is a remote system administration tool over SSH for low quality connection (3G, Modem ...)

ras is nothing more than a wrapper to call some ssh command from a
remote server. For each command, ras will establish a SSH connection,
launch a command and return its result.

ras is in alpha version and still buggy at this point. It does not
support yet command who require terminal information such as mutt or
slrn.

To edit a file, ras is using vim and vim only. If you like Emacs, ras is
not a tool for you (anyway, if you use Emacs, you don't need any other
tool than Emacs). Just type 'vim [file]' and it will launch a local
instance of vim with remote content of the file.

First time you start ras, it will create minimal configuration file in
~/.ras/config. Edit this file and add your own servers. Then restart
ras, a menu will display available servers with user. Just hit it's
number and you're connected, you can now use any non interactive Unix
command.

Even I'm using it my self to reach some server over a dumb 3G
connection, ras is NOT a very serious software. First it's using C
because I'm learning C, but it would be much smarter to write it in Perl
(I wrote the version in Perl) or even in shell.

To install it, just compile it with 'make' and copy the binary file
where ever fits you.

You have a question, you think ras is a great software and you want some
features, you notices a bug or you just want to say 'Hi'. Send me an
email to stephane@unices.org


