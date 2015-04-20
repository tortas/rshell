#rshell 

rshell is a simplified version of the bash command shell. 

###GENERAL USAGE:
-------------------

Commands should have the form:
```
cmd			 =     executable   [argumentList] [connector]
connector    =   || or && or ;	
```
where `executable` is an executable program in the `PATH` and `argumentList` is a list of zero or 
more arguments separated by spaces. Connectors allow you to run multiple commands at once.
	If a command is followed by `;`, then the next command is always executed;
	if a command is followed by `&&`, then the next command is executed only if the first one succeeds;
	if a command is followed by `||`, then the next command is executed only if the first one fails;

For example:
```
ls -a; pwd; echo hello
```
is equivalent to 
```
ls -a
pwd
echo hello
```

Anything after a `#` is considered a comment and will be disregarded by the shell.


###INSTALLATION
This program can only be run by machines running Linux. From your working directory run the following commands:
```
$ git clone  https://github.com/tortas/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```

###BUGS
rshell contains the following bugs:
	1. rshell does not support any sort of input redirection
	2. rshell will not continue execution if it encounters unknown connectors (i.e. `&&&` or `|||`)
	3. rshell disregards singular `&` and `|` connectors which may cause errors

Any other bugs should be posted as an issue in the github repository:
	`https://github.com/tortas/rshell.git`
=================================================================================================
