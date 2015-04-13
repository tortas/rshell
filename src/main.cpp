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
	char usrIn[128];
	char *token;
	cout << "% ";
	fgets(usrIn, 128, stdin);
	token = strtok(usrIn, " ");

	while (strcmp(usrIn, "exit\n") != 0)
	{
		cout << "% ";
		fgets(usrIn,128,stdin);
	}

	return 0;
}
