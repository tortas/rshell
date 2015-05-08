CXX = g++
FLAGS = -Wall -Werror -pedantic

all: rshell ls rm

rshell: bin 
	$(CXX) $(FLAGS) src/main.cpp -o bin/rshell

ls: bin
	$(CXX) $(FLAGS) src/ls.cpp -o bin/ls

rm: bin
	$(CXX) $(FLAGS) src/rm.cpp -o bin/rm

bin:
	mkdir bin

clean:
	rm -rf bin/
