#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

int main()
{
	while(1){
		cout << "$ ";
		char usrString[256];
		cin.getline(usrString, 256, '\n');
		char *args[256];
		char *temp = strtok(usrString, " ");
		int cnt = 0;
		while (temp != 0)
		{
			args[cnt] = temp;
			++cnt;
			temp = strtok(NULL, " ");
		}
		args[cnt] = NULL;
		cout << args[0] << endl;
		int pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(1);
		}
		else if(pid == 0){
			if(-1 == execvp(args[0], args)){
				perror("execvp");
			}
		}
		else if(pid > 0)
		{
			if ( -1 == wait(0)){
				perror("wait");
			}
		}
	}
	return 0;
}

