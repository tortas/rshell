CXX = g++
FLAGS = -Wall -Werror -pedantic

all: rshell ls

rshell: bin 
	$(CXX) $(FLAGS) src/main.cpp -o bin/rshell

ls: bin
	$(CXX) $(FLAGS) src/ls.cpp -o bin/ls

bin:
	mkdir bin

clean:
	rm -rf bin/
