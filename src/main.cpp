#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

using namespace std;
	
void parse_input(char* (&args)[100], string usrString){
	char *cstr = new char[usrString.length()+1];
	strcpy(cstr,usrString.c_str());
	char *temp = strtok(cstr, " &|");
	int cnt = 0;

	while (temp != 0){
		args[cnt] = temp;
		++cnt;
		temp = strtok(NULL, " &|");
	}
	args[cnt] = NULL;
	for(int i = 0; i < cnt; i++){
		cout << args[i] << endl;
	}
	delete[] cstr;
	return;
} 

//Function checks for connectors and returns a number based on which one is found
//Pass in a string, a position integer, and a starting point integer
int check_connect(const string& str, int& pos, int start){
	unsigned found = 0;
	found = str.find_first_of(";&|#",start);
	if (found >= str.size()){	//If not found...
		pos = -1;
		return -1;
	}
	else{
		if (str.at(found) == ';'){
			pos = found;
			return 1;
		}
		else if (str.at(found) == '&'){
			if (str.at(found+1) == '&'){
				pos = found;
				return 2;
			}
			else{
				return check_connect(str,pos,found+1);
			}
		}

		else if (str.at(found) == '|'){
			if (str.at(found+1) == '|'){
				pos = found;
				return 3;
			}
			else{
				return check_connect(str,pos,found+1);
			}
		}
		else if (str.at(found) == '#'){
			pos = found;
			return 4;
		}
	}
	return -1;
}

int main()
{
	int start = 0;
//	int c_stat;
//	int pos = -1;
	vector<string> str;
	str.push_back("ls -a");
	str.push_back("pwd");
	str.push_back("&Third: && got that ampersand");
	str.push_back("We has | in i||t");
	str.push_back("This has a #comment");
	char **args = new char*[100];
	parse_input(*args,str.at(0));
	cout << "command: " << args[0] << endl;
	int pid = fork();
	if (pid == -1){
		perror("fork");
	}
	else if (pid == 0){
		if(execvp(args[0],args) == -1){
			perror("execvp");
		}
		exit(0);
	}
	else if(pid > 0){
		if(wait(0) == -1){
			perror("wait");
		}
		cout << "Done!" << endl;
	}
		
//	for (int i = 0; i < 5; ++i){
//		parse_input(args,str.at(i));
//	}
	delete[] args;
	return 0;
}


