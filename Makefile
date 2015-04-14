all: rshell
	mkdir bin; mv rshell bin

rshell: 
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o rshell
