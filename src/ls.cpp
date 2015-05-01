#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <errno.h>
#include <stdio.h>
#include <cctype>
using namespace std;

bool compareNoCase (string first, string second)
{
	unsigned i = 0;
	while ((i<first.length()) && (i<second.length()))
	{
		if (tolower(first[i]) < tolower(second[i]))
		{
			return true;
		}	
		else if (tolower(first[i]) > tolower(second[i]))
		{
			return false;
		}
		i++;
	}
	if(first.length() < second.length())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void reg_ls (vector<string> &f_names)
{
	DIR* dirp;
    if (NULL == (dirp = opendir( "." )))
    {
    	perror( "opendir()" );
    	exit(1);
    }
                                                
    struct dirent* filespecs;
    errno = 0;
    while (NULL != (filespecs = readdir(dirp)))
    {
    	f_names.push_back(filespecs->d_name);
    }
    
    sort (f_names.begin(), f_names.end(), compareNoCase);

    for (unsigned i = 0; i<f_names.size(); i++)
    {
    	if (f_names.at(i).at(0) != '.')
    	{
    		cout << f_names.at(i) << "  ";
    	}
    }
                                                
                                                
    if (errno != 0)
    {
    	perror( "readdir()" );
    	exit(1);
    }
    cout << endl;
                                                
    if (-1 == closedir(dirp))
    {
    	perror ("closedir()" );
    	exit(1);
    }
}

void all_ls (vector<string> &f_names)
{                                               		
	DIR* dirp;                                  		
    if (NULL == (dirp = opendir( "." )))
    {
    	perror( "opendir()" );
    	exit(1);
    }
                                                
    struct dirent* filespecs;
    errno = 0;
    while (NULL != (filespecs = readdir(dirp)))
    {
    	f_names.push_back(filespecs->d_name);
    }
    
    sort (f_names.begin(), f_names.end(), compareNoCase);

    for (unsigned i = 0; i<f_names.size(); i++)
    {
    	cout << f_names.at(i) << "  ";
    }
                                                
                                                
    if (errno != 0)
    {
    	perror( "readdir()" );
    	exit(1);
    }
    cout << endl;
                                                
    if (-1 == closedir(dirp))
    {
    	perror ("closedir()" );
    	exit(1);
    }
}



int main( int argc, char* argv[])
{
	vector<string> f_names;
	vector<string> args(argv, argv+argc);
	vector<string> files;
	vector<string> flags;
	for( int i = 1; i < argc; i++ )
	{
		string temp = args.at(i);
		if(temp.at(0) == '-')
		{
			flags.push_back(temp);
		}
		else
		{
			files.push_back(temp);
		}
	}
	
	cout << "flags: " << endl;
	for (unsigned j = 0; j < flags.size(); j++)
	{
		cout << flags.at(j) << endl;
	}
	cout << "files: " << endl;
	for (unsigned k = 0; k < files.size(); k++)
	{
		cout << files.at(k) << endl;
	}
	if (argc < 2)
	{
		reg_ls(f_names);
	}


	return 0;
}
