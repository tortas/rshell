#include <string.h>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

using namespace boost;
using namespace std;

int check_input(string usrString)
{
	if (usrString.find("&&") != string::npos ||
		usrString.find("||") != string::npos ||
		usrString.find(";") != string::npos ||
		usrString.find("#") != string::npos ||
		(usrString.find(">") == string::npos &&
		usrString.find("<") == string::npos &&
		usrString.find("|") == string::npos))
		{
			return 0;
		}
	else
	{
		return 1;
	}
	return -1;
}

bool popv(string usrString, vector<string> &iop, vector<vector<string> > &cmds)
{
	typedef tokenizer<char_separator<char> > tokenizer;
	char_separator<char> sep(" ", "<>|", drop_empty_tokens);
	tokenizer tokens(usrString, sep);
	int cnt = 0;
	bool first = true;
	for (auto it = tokens.begin(); it != tokens.end(); ++it)
	{
		if(*it == "|" || *it == "<")
		{
			first = true;
			iop.push_back(*it);
			++cnt;
			continue;
		}
		else if(*it == ">")
		{
			if(iop.empty())
            {
            	first = true;
            	iop.push_back(*it);
            	++cnt;
            	continue;
            }

			else if (iop.at(iop.size()-1) == ">")
			{
				first = true;
				iop.at(iop.size()-1).append(">");
				continue;
			}
			else
			{
				first = true;
				iop.push_back(*it);
				++cnt;
			}
		}
		else
		{
			if(first)
			{
				first = false;
				vector<string> temp;
				temp.push_back(*it);
				cmds.push_back(temp);
			}
			else
			{
				cmds.at(cnt).push_back(*it);
			}
		}

	}

	//Print check
	cout << "cmds: ";
	for (size_t i = 0; i < cmds.size(); ++i)
	{
		for(size_t j = 0; j < cmds.at(i).size(); ++j)
		{
			cout << cmds.at(i).at(j) << " ";
		}
	}
	cout << endl;
	
	cout << "iop: ";
	for (size_t n = 0; n < iop.size(); ++n)
	{
		cout << iop.at(n) << " ";
	}
	cout << endl;
	return true;
}

	
//Function that parses a passed in string and executes the command it yields
bool parse_exec(string usrString, int size)
{
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
		delete[] cstr;
		delete[] args;
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
				return 5; 
			}
		}
		else if (str.at(found) == '#'){
			pos = found;
			return 4;
		}
	}
	return -1;
}

void my_close(int fd)
{
	if (-1 == close(fd))
	{
		perror("close");
	}
}
void redirect(int oldf, int newf)
{
	if (oldf != newf)
	{
		if (-1 == dup2(oldf,newf))
		{
			perror("dup2");
		}
		else
		{
			my_close(oldf);
		}
	}
}

void execute(vector<string> cmd, int in, int out)
{
	redirect(in, STDIN_FILENO);
	redirect(out, STDOUT_FILENO);

	int n = cmd.size();
	int i;
	const char **args = new const char*[ n+1 ];
	for (i = 0; i < n; ++i)
	{
		args[i] = cmd.at(i).c_str();
	}
	args[i] = NULL;

	execvp(args[0], (char * const*)args);
	perror("execvp");
	exit(1);
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
		vector<string> iop;
		vector<vector<string> > cmds;
		cout << username << "@" << hostname << "$ ";
		cin.sync();
		getline(cin,usrString); size = usrString.size();
		if (size == 0){
			continue;
		}

		if (1 == check_input(usrString))
		{
			cout << "didn't find connectors. start piping" << endl;
			iop.clear(); cmds.clear();
			if(popv(usrString,iop,cmds))
			{
				cout << "YES!" << endl;
			}

			int pipe_cnt = 0;
			for (size_t a = 0; a < iop.size(); ++a)
			{
				if (iop.at(a) == "|")
				{
					++pipe_cnt;
				}
			}

			if(cmds.empty())
			{
				continue;
			}

			int save_in;
			save_in = dup(STDIN_FILENO);
			pid_t pid;
			int in = STDIN_FILENO;
			size_t i;
			int fd[2];
			for (i = 0; i < cmds.size()-1; ++i)
			{
				
				if (-1 == pipe(fd))
				{
					perror("pipe");
					exit(1);
				}
				else if (-1 == (pid = fork()))
				{
					perror("fork");
					exit(1);
				}
				else if (pid == 0)
				{
					my_close(fd[0]);
					execute(cmds.at(i),in,fd[1]);
					exit(1);
				}
				else if (pid >0)
				{
					wait(0);
					my_close(fd[1]);
					my_close(in);
					in = fd[0];
				}
			}
			
			if (-1 == (pid = fork()))
			{
				perror("fork");
				exit(1);
			}
			if (pid == 0)
			{
				execute(cmds.at(i),in,STDOUT_FILENO);
				exit(1);
			}
			
			wait(0);
			dup2(save_in,STDIN_FILENO);
				
		//	return 0;




		}

		else
		{
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
	}
	return 0;

}
	
	
