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
	
bool parse_exec(string usrString, int size){
	int status;
	char **args = new char*[size+1];
	char *cstr = new char[size+1];
	strcpy(cstr,usrString.c_str());
	char *temp = strtok(cstr, " &|;#");
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

	if (!strcmp(args[0],"exit")){
		exit(0);
	}

	int pid = fork();
	if (pid == -1){
		perror("fork");
	}
	else if(pid == 0){
		if (execvp(args[0],args) == -1){
			perror("execvp");
		}
		exit(0);
	}
	else if(pid > 0){
		if(-1 == wait(&status)){
			perror("wait");
		}
	}

	delete[] cstr;
	delete[] args;

	if(WIFEXITED(status) != 0){
		cout << "Success!" << endl;
		return true;
	}
	else{
		cout << "Failure!" << endl;
		return false;
	}
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
	//Get User Info
	


	while(1){
		//Get User Input
		bool exec_stat = true;		//Connector Status
		int con_stat = 0;
		int pos = 0;
		int start = 0;
		int size = 0;
		string usrString;
		cout << "$ ";
		getline(cin,usrString); size = usrString.size();
		if (size == 0){
			continue;
		}
		

		//Check Connectors
		while(1){
			con_stat = check_connect(usrString,pos,start);
			if (con_stat == -1){
				exec_stat = parse_exec(usrString.substr(start),size);
				break;
			}
			else if (con_stat == 1){
				exec_stat = parse_exec(usrString.substr(start,pos), size);
				start = pos + 1;
				continue;
			}
			else if (con_stat == 2){
				exec_stat = parse_exec(usrString.substr(start,pos), size);
				start = pos + 2;
				if (exec_stat == true){
					continue;
				}
				else{
					break;
				}
			}
			else if (con_stat == 3){
				exec_stat = parse_exec(usrString.substr(start,pos), size);
				start = pos + 2;
				if (exec_stat == true){
					break;
				}
				else{
					continue;
				}
			}
			else if (con_stat == 4){
				exec_stat = parse_exec(usrString.substr(start,pos), size);
				break;
			}
		}
	}
		return 0;

}
	//	while (pos != -1){	
	//		//Parsing and Executing Section
	//		char **args = new char*[usrString.length()+1];
	//		char *cstr = new char[usrString.length()+1];
	//		strcpy(cstr,usrString.c_str());
	//		char *temp = strtok(cstr, " &|");
	//		int cnt = 0;                                
	//		                                            
	//		while (temp != 0){                          
	//			args[cnt] = temp;                       
	//			++cnt;                                  
	//			temp = strtok(NULL, " &|");             
	//		}                                           
	//		args[cnt] = NULL;                           
	//		for(int i = 0; i < cnt; i++){               
	//			cout << args[i] << endl;                
	//		}                                           
	//			
	//		//Execution
	//		if(!strcmp(args[0], safe_word)){		//Check for exit command
	//			exit(0);
	//		}
	//	
	//		int pid = fork();
	//		 if (pid == -1){
	//			perror("fork");
	//		}
	//		else if (pid == 0){
	//			if(execvp(args[0],args) == -1){
	//				perror("execvp");
	//			}
	//			exit(0);
	//		}
	//		else if(pid > 0){
	//			if(wait(0) == -1){
	//				perror("wait");
	//			}
	//		}
	//		
	//
	//		delete[] cstr;                              
	//		delete[] args;
	//		}
