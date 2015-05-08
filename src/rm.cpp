#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <dirent.h>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
using namespace std;

void populatev (vector<string> &args, vector<string> &flags,
	vector<string> &files, int cnt)
{
	for (int i = 1; i < cnt; i++) 
	{
		string temp = args.at(i);
		if (temp.at(0) == '-')
		{
			flags.push_back(temp);
		}
		else
		{
			files.push_back(temp);
		}
	}
	return;
}

void check_set_flags (vector<string> &flags, bool &r_flag)
{
	for(unsigned i = 0; i < flags.size(); i++)
	{
		string temp = flags.at(i);

		for(unsigned j = 1; j < temp.size(); j++)
		{
			char temp2 = temp.at(j);
			if (temp2 == 'r')
			{
				r_flag = true;
			}
			else
			{
				cout << "Invalid flag" << endl;
				exit(1);
			}
		}
	}
}

void rm (const char *file)
{
	struct stat filespec;
	bool is_dir = false;

	if (-1 == stat(file,&filespec))
	{
		perror("stat");
		exit(1);
	}
	
	is_dir = S_ISDIR(filespec.st_mode);

	if (is_dir)
	{
		DIR *dirp;
		if (NULL == (dirp = opendir(file)))
		{
			perror("opendir");
			exit(1);
		}

		struct dirent* dirspecs;
		errno = 0;
		while (NULL != (dirspecs = readdir(dirp)))
		{
			if (strcmp(dirspecs->d_name, ".") == 0 || strcmp(dirspecs->d_name, "..") == 0)
			{
				continue;
			}
			string next = file;
			next.append("/");
			next.append(dirspecs->d_name);
			rm(next.c_str());
		}

		if (errno != 0)
		{
			perror("readdir");
			exit(1);
		}

		if (-1 == closedir(dirp))
		{
			perror("closedir");
			exit(1);
		}

		if (-1 == rmdir(file))
		{
			perror("rmdir");
		}
	}

	else
	{
		unlink(file);
	}
}
int main (int argc, char *argv[])
{
	vector<string> args(argv,argv+argc);
	vector<string> files;
	vector<string> flags;
	bool r_flag = false;
	bool is_dir = false;

	if (argc < 2)
	{
		exit(0);
	}

	populatev(args,flags,files, argc);
	check_set_flags(flags,r_flag);

	for (unsigned i = 0; i < files.size(); i++)
	{
		string curr = files.at(i);
		struct stat filespec;
		if (-1 == stat(curr.c_str(),&filespec))
		{
			perror("stat");
			exit(1);
		}
		
		is_dir = S_ISDIR(filespec.st_mode);
		if (is_dir && !r_flag)
		{
			cerr << "Warning: Attempting to remove directory" << endl;
			continue;
		}
		rm(curr.c_str());
	}

	return 0;
}
		

