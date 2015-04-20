all: rshell

rshell: 
	mkdir bin
	g++ --std=c++11 -Wall -Werror -ansi -pedantic src/main.cpp -o bin/rshell
