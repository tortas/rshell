all: rshell

rshell: 
	g++ -Wall -Werror -ansi -pedantic src/main.cpp -o rshell
