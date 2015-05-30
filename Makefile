CXX = g++
C11 = --std=c++11
FLAGS = -Wall -Werror -pedantic

all: rshell 

rshell: bin 
	$(CXX) $(C11) $(FLAGS) src/main.cpp -o bin/rshell

ls: bin
	$(CXX) $(FLAGS) src/ls.cpp -o bin/ls

rm: bin
	$(CXX) $(FLAGS) src/rm.cpp -o bin/rm

mv: bin
	$(CXX) $(FLAGS) src/mv.cpp -o bin/mv
bin:
	mkdir bin

clean:
	rm -rf bin/
