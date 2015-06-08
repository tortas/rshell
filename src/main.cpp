#include <string.h>
#include <signal.h>
#include <fcntl.h>
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
	/*
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
	*/
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
		pid_t wpid;
		do
		{
			wpid = wait(&status);
		} while (wpid == -1 && errno == EINTR);
		if (-1 == wpid)
		{
			perror("wait");
			return 1;
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

void in_redir(vector<string> cmd, string file, int out, int cnt)
{
	my_close(0);
	if (-1 == open(file.c_str(), O_RDWR))
	{
		perror("open");
		exit(1);
	}
	size_t i;
	const char **args = new const char*[cmd.size()+1];
	for (i = 0; i < cmd.size(); ++i)
	{
		args[i] = cmd.at(i).c_str();
	}
	args[i] = NULL;
	if ( cnt > 0 )
	{
		redirect(out,STDOUT_FILENO);
	}
	if(-1 == execvp(args[0], (char * const*)args))
	{
		perror("execvp");
		exit(1);
	}
}

bool cd_check(string usrString, vector<string> &cd_vec)
{
	typedef tokenizer<char_separator<char> > tokenizer;
	char_separator<char> sep(" ");
	tokenizer tokens(usrString, sep);

	int cnt = 0;
	for (auto it = tokens.begin(); it != tokens.end(); ++it)
	{
		if ((cnt == 0) && (*it != "cd"))
		{
			return false;
		}
		cnt++;
		cd_vec.push_back(*it);
	}
		
	return true;
}

bool cd_action(vector<string> &cd_vec, char *curr)
{
	if (cd_vec.size() == 1)
	{
		if(-1 == chdir(getenv("HOME")))
		{
			perror("chdir: HOME:");
			return false;
		}
		if(-1 == setenv("PWD",getenv("HOME"),1))
		{
			perror("setenv");
		}
		if(-1 == setenv("OLDPWD",curr,1))
		{
			perror("setenv");
		}
		return true;
	}
	else if (cd_vec.at(1) == "-")
	{	
		if(-1 == chdir(getenv("OLDPWD")))
		{
			perror("chdir: OLDPWD:");
			return false;
		}
		if(-1 == setenv("PWD",getenv("OLDPWD"),1))
		{
			perror("setenv");
		}
		if(-1 == setenv("OLDPWD",curr,1))
		{
			perror("setenv");
		}
		return true;
	}
	else
	{
		const char *dir_to = cd_vec.at(1).c_str();
		if(-1 == chdir(dir_to))
		{
			perror("chdir");
			return false;
		}
		char cwd[PATH_MAX];
		if(NULL == getcwd(cwd,PATH_MAX))
		{
			cerr << "Error with getcwd()" << endl;
		}
		if(-1 == setenv("PWD", cwd, 1))
		{
			perror("setenv");
		}
		if(-1 == setenv("OLDPWD", curr, 1))
		{
			perror("setenv");
		}
		return true;
	}

	return false;	
}
pid_t parent_pid;

void sighdl (int signum, siginfo_t *siginfo, void *context)
{
	if (signum == SIGINT)
	{
		cout << endl;
		pid_t self = getpid();
		if(parent_pid != self)
		{
			_exit(0);
		}
	}
}

int main()
{
	//Setup sigaction 
	struct sigaction act;

	act.sa_sigaction = &sighdl;
	act.sa_flags = SA_SIGINFO;

	if (sigaction(SIGINT, &act, NULL) < 0)
	{
		perror("sigaction");
		return 1;
	}
	parent_pid = getpid();

	//Set OLDPWD to home 
	char *home = getenv("HOME");
	string home_s = home;
	if(home == NULL)
	{
		cout << "HOME not found" << endl;
	}
	if (-1 == setenv("OLDPWD",home,1))
	{
		perror("setenv");
	}
	//Get User Info
	string hostname;
	char* username;
	username = getlogin();
	if(username == NULL){
		perror("login");
	}
	char host_temp[64];
	if(gethostname(host_temp,64) == -1){
		perror("hostname");
		hostname = "UNKNOWN";
	}
	else{
		hostname = host_temp;
	}


	string squig = "~";
	while(1){
		//Initialize Variables
		bool exec_stat = true;		//Execution Status
		int con_stat = 0;			//Connector Status
		int pos = 0;				//Position of Connector
		int start = 0;				//Start position for parse function
		int size = 0;				//Size of user input
		string usrString;			//User Input String
		bool cd_flag = false;
		bool o_redir1 = false;
		bool o_redir2 = false;
		bool i_redir = false;
		vector<string> cd_vec;
		vector<string> iop;
		vector<vector<string> > cmds;

		//Print Prompt
		char *cwd = getenv("PWD");
		string cwd_str = cwd;
		if (0 == cwd_str.find(home_s,0))
		{
			cwd_str.replace(0,home_s.length(),squig);
		}
		
		cout << username << "@" << hostname << ":" << cwd_str << "$ ";

		//Get User Input
		cin.clear();
		getline(cin,usrString); size = usrString.size();
		if (size == 0){
			continue;
		}

		cd_vec.clear();
		cd_flag = cd_check(usrString, cd_vec);
		if (cd_flag)
		{
			if (!cd_action(cd_vec,cwd))
			{
				cout << "error with cd_action" << endl;
			}
			continue;
		}

		if (1 == check_input(usrString))
		{
			iop.clear(); cmds.clear();
			popv(usrString,iop,cmds);

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

			if (iop.at(iop.size()-1) == ">")
			{
				o_redir1 = true;
			}
			else if (iop.at(iop.size()-1) == ">>")
			{
				o_redir2 = true;
			}
			if (iop.at(0) == "<")
			{
				i_redir = true;
			}

			int save_out;
			if (-1 == (save_out = dup(STDOUT_FILENO)))
			{
				perror("dup");
				exit(1);
			}  

			int save_in;
			if ( -1 == (save_in = dup(STDIN_FILENO)))
			{
				perror("dup");
				exit(1);
			}

			pid_t pid;
			int in = STDIN_FILENO;
			size_t i = 0;
			int fd[2];
			if ((i_redir) && (o_redir1 || o_redir2) && (pipe_cnt == 0))
			{
				int infd = 0;
				int outfd = 0;
				string in_file = cmds.at(1).at(0);
				string out_file = cmds.at(2).at(0);
				if (-1 == (infd = open(in_file.c_str(), O_RDONLY)))
				{
					perror("open infile");
				}
				if (o_redir1)
				{
					if (-1 == (outfd = open(out_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT,
						S_IRUSR | S_IRGRP| S_IWUSR | S_IWGRP)))
					{
						perror("open outfile");
					}
				}
				else if(o_redir2)
				{
					if (-1 == (outfd = open(out_file.c_str(), O_WRONLY | O_APPEND | O_CREAT,
						S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP)))
					{
						perror("open outfile");
					}
				}

				if (-1 == (pid = fork()))
				{
					perror("fork");
					return 1;
				}
				if (pid == 0)
				{
					execute(cmds.at(0),infd,outfd);
				}
				if (pid > 0)
				{
					if (-1 == wait(0))
					{
						perror("wait");
					}
					redirect(save_in,STDIN_FILENO);
					redirect(save_out,STDOUT_FILENO);
				}
				continue;
			}


			if (i_redir)
			{
				if (-1 == pipe(fd))
				{
					perror("pipe");
				}
				//REMOVE AFTER TESTING
				cout << "fd[0] " << fd[0] << endl;
				cout << "fd[1] " << fd[1] << endl;
				if (-1 == (pid = fork()))
				{
					perror("fork");
					exit(1);
				}
				else if (pid == 0)
				{
					my_close(fd[0]);
					in_redir(cmds.at(0),cmds.at(1).at(0),fd[1],pipe_cnt);
				}
				else if (pid > 0)
				{
					if (-1 == wait(0))
					{
						perror("wait");
						exit(1);
					}
					my_close(fd[1]);
					my_close(in);
					in = fd[0];
					i = 2;
				}
			}

			if ((cmds.size() >= 2) && (pipe_cnt > 0 || o_redir1 || o_redir2))
			{
				int k = 1;
				if (o_redir1 || o_redir2)
				{
					++k;
				}

				for (; i < cmds.size()-k; ++i)
				{
					cout << "stdout: " << STDOUT_FILENO << endl; //TEST
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
						if (-1 == wait(0))
						{
							perror("wait");
							exit(1);
						}
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
					if (o_redir1 || o_redir2)
					{
						int ofile_fd;
						string outfile = cmds.at(cmds.size()-1).at(0);
						cout << "outfile: " << outfile << endl; //TEST
						if (o_redir1)
						{
							if (-1 == (ofile_fd = open(outfile.c_str(),	O_CREAT | O_TRUNC | O_RDWR)))
							{
								perror("open");
							}
						}
						else if (o_redir2)
						{
							if (-1 == (ofile_fd = open(outfile.c_str(),	O_CREAT | O_RDWR | O_APPEND)))	
                            {
                            	perror("open");
                            }
						}
						redirect(ofile_fd, STDOUT_FILENO);
						execute(cmds.at(i),in,STDOUT_FILENO);
					}
					else
					{
						cout << "stdout for last: " << STDOUT_FILENO << endl; //TEST
						execute(cmds.at(i),in,STDOUT_FILENO);
						exit(1);
					}
				}
				else if (pid > 0)
				{
					if (-1 == wait(0))
					{
						perror("wait");
						exit(1);
					}
					my_close(in);
					in = fd[0];
					//REMOVE AFTER TESTING
					cout << "fd[0]: " << fd[0] << endl;
					cout << "fd[1]: " << fd[1] << endl;
				}
			}
			redirect(save_in,STDIN_FILENO);
			if (o_redir1 || o_redir2)
			{
				cout << "save out close" << endl; //TEST
				redirect(save_out,STDOUT_FILENO);
			}
			continue;
				
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
	
	
