#rshell 

rshell is a simplified version of the bash command shell. 

###GENERAL USAGE:


Commands are executed from left to right and should have the form:
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

####Input Redirection and Piping

rshell now supports input redirection from files (`<`) and piping (`|`)!
Input redirection must be done first if combined with piping.
For example:
```
cat < testfile.txt | grep keyword
```
You can connect many commands with pipes like:
```
lsof | grep something | tail -5 | tee newfile
```

####`cd` Command
rshell now supports the `cd` command! 
You may use the following forms with `cd`:

1. `cd <PATH>` will change the current working directory to `<PATH>`
2. `cd` will change the current working directory to the user's home directory
3. `cd -` will change the current working directory to the previous working directory

Currently, `cd` only works by itself.
It may not be combined with any other commands via connectors.
`cd` recognizes only one `<PATH>` option.
Entering something like: `cd src other` will only change the directory to src.

####Signals
rshell now recognizes `^C` signal. 
If a job is running in rshell and the signal is sent, that job is killed.
Control is then returned to the shell for more input.
If there is no job running in rshell and the signal is sent, nothing happens.
Now if you open an executable that happens to send your terminal into chaos, maybe `^C` will help!

###INSTALLATION
This program can only be run by machines running Linux. 
From your working directory run the following commands:
```
$ git clone  https://github.com/tortas/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```

###BUGS
rshell contains the following bugs:

1. rshell does not support any sort of output redirection
2. rshell will not continue execution if it encounters unknown connectors (i.e. `&&&` or `|||`)
3. rshell disregards singular `&` connectors which may cause errors
4. `cd -` moves you up the directory tree like `cd ..` does in bash.

Any other bugs should be posted as an issue in the github repository:
	``https://github.com/tortas/rshell.git``
