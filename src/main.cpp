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
	
//Function that parses a passed in string and executes the command it yields
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
	if (cnt == 0){
		return false;
	}
	args[cnt] = NULL;

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
			exit(1);
		}
	}
	else if(pid > 0){
		if(-1 == wait(&status)){
			perror("wait");
		}
	}

	delete[] cstr;
	delete[] args;
	
	if(WEXITSTATUS(status) == 0){
		return true;
	}
	else{
		return false;
	}
	return false;
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
	string hostname;
	string username = getlogin();
	if(getlogin() == NULL){
		perror("login");
		username = "UNKNOWN";
	}
	char host_temp[64];
	if(gethostname(host_temp,64) == -1){
		perror("hostname");
		hostname = "UNKNOWN";
	}
	else{
		hostname = host_temp;
	}


	while(1){
		//Get User Input
		bool exec_stat = true;		//Execution Status
		int con_stat = 0;			//Connector Status
		int pos = 0;				//Position of Connector
		int start = 0;				//Start position for parse function
		int size = 0;				//Size of user input
		string usrString;			//User Input String
		cout << username << "@" << hostname << "$ ";
		getline(cin,usrString); size = usrString.size();
		if (size == 0){
			continue;
		}
		

		//Check Connectors
		while(1){
			exec_stat = false;
			con_stat = check_connect(usrString,pos,start);
			if (con_stat == -1){
				exec_stat = parse_exec(usrString.substr(start),size);
				break;
				//If -1 there are no more connectors in the string so
				//execute and break for more input
			}
			else if (con_stat == 1){
				exec_stat = parse_exec(usrString.substr(start,pos-start), size);
				start = pos + 1;
				continue;
				//Semi-colon encountered so execute and continue with 
				//the rest of command line
			}
			else if (con_stat == 2){
				exec_stat = parse_exec(usrString.substr(start,pos-start), size);
				start = pos + 2;
				if (exec_stat == true){
					continue;
				}
				else{
					break;
				}
				//AND connector encountered so execute and check it's exit status
				//If success then continue with command line
				//Else break for more input
			}
			else if (con_stat == 3){
				exec_stat = parse_exec(usrString.substr(start,pos-start), size);
				start = pos + 2;
				if (exec_stat == true){
					break;
				}
				else{
					continue;
				}
				//OR connector encountered so execute and check it's exit status
				//If success then break for more input
				//Else continue with command line
			}
			else if (con_stat == 4){
				exec_stat = parse_exec(usrString.substr(start,pos-start), size);
				break;
				//COMMENT encountered so execute everything before and break
				//for more input
			}
		}
	}
		return 0;

}
	
	
