#include <string.h>
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
	cout << "$ ";
	char usrString[256];
	cin.getline(usrString, 256, '\n');
	char *args[256];
	char *temp = strtok(usrString, " \n;");
	int pos = 0;
	int cnt = 0;
	while (temp != 0)
	{
		args[pos] = temp;
		++pos;
		++cnt;
		temp = strtok(NULL, " ");
	}
	args[cnt] = NULL;
	cout << args[0] << endl;
	if(-1 == execvp(args[0], args)){
		perror("execvp");
	}

	for (int i = 0; i < cnt; ++i)
	{
		cout << "token: "<< args[i] << endl;
	}	
	return 0;
}

