#include <iostream>
#include <iomanip>
#include <grp.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <cstdlib>
#include <errno.h>
#include <stdio.h>
#include <cctype>
#include <pwd.h>
using namespace std;

struct fileInfo
{
	string fperm;
	int lnks;
	string usr;
	string grp;
	int sz;
	string date_time;
	bool hidden;
	string fname;
};

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

void read_cdir (vector<string> &f_names)
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

    if (errno != 0)
    {
    	perror( "readdir()" );
    	exit(1);
    }
                                                
    if (-1 == closedir(dirp))
    {
    	perror ("closedir()" );
    	exit(1);
    }
}

int main( int argc, char* argv[])
{
	bool a_flag=false, l_flag=false;
	vector<string> f_names;
	vector<string> args(argv, argv+argc);
	vector<string> files;
	vector<string> flags;

    read_cdir(f_names);
	if (argc < 2)
    {
    	string temp;
    	for( unsigned cnt = 0; cnt < f_names.size(); cnt++ )
    	{
    		temp = f_names.at(cnt);
    		if (temp.at(0) != '.')
    		{
    			cout << temp << "  ";
    		}
    	}
    	cout << endl;
    	return 0;
    }
	
	//Populate flags and file vectors
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
	
	//Check flags and set bools
	for( unsigned pos = 0; pos < flags.size(); pos++)
	{
		string temp = flags.at(pos);
		for( unsigned pos2 = 1; pos2 < temp.size(); pos2++)
		{
			char temp2 = temp.at(pos2);
			if( temp2 == 'a' )
			{
				a_flag = true;
			}
			else if( temp2 == 'l' )
			{
				l_flag = true;
			}
			else
			{
				cout << "ls: invalid option -- '" << temp2 << "'" << endl;
				exit(1);
			}
		}
	}
	
	vector<fileInfo> vfI;
	vector<int> nlks;
	vector<int> fsizes;
	struct stat sb;
	int totalBlks = 0;
	if (l_flag)
	{
		struct fileInfo fI;
		unsigned cnt;
		for (cnt = 0; cnt < f_names.size(); cnt++)
		{
			if (stat(f_names.at(cnt).data(), &sb) == -1)
			{
				perror( "stat" );
				continue;
			}
			fI.fperm.clear();
			switch (sb.st_mode & S_IFMT)
			{
				case S_IFBLK: fI.fperm += 'b';			break;
				case S_IFCHR: fI.fperm += 'c';			break;
				case S_IFDIR: fI.fperm += 'd';			break;
				case S_IFLNK: fI.fperm += 'l';			break;
				case S_IFREG: fI.fperm += '-';			break;
			}
			(sb.st_mode & S_IRUSR) ? (fI.fperm += 'r') : (fI.fperm += '-');
			(sb.st_mode & S_IWUSR) ? (fI.fperm += 'w') : (fI.fperm += '-');
			(sb.st_mode & S_IXUSR) ? (fI.fperm += 'x') : (fI.fperm += '-');
			(sb.st_mode & S_IRGRP) ? (fI.fperm += 'r') : (fI.fperm += '-');
            (sb.st_mode & S_IWGRP) ? (fI.fperm += 'w') : (fI.fperm += '-');
            (sb.st_mode & S_IXGRP) ? (fI.fperm += 'x') : (fI.fperm += '-');
			(sb.st_mode & S_IROTH) ? (fI.fperm += 'r') : (fI.fperm += '-');
            (sb.st_mode & S_IWOTH) ? (fI.fperm += 'w') : (fI.fperm += '-');
            (sb.st_mode & S_IXOTH) ? (fI.fperm += 'x') : (fI.fperm += '-');
			fI.lnks = sb.st_nlink;
			nlks.push_back(sb.st_nlink);
			passwd *pwd = getpwuid(sb.st_uid);
			fI.usr = pwd->pw_name;
			group *grpnm = getgrgid(sb.st_gid);
			fI.grp = grpnm->gr_name;
			fI.sz = sb.st_size;
			fsizes.push_back(sb.st_size);
			fI.fname = f_names.at(cnt); 
			totalBlks += sb.st_blocks;
			char buffer[80];
			struct tm* timeinfo = localtime(&sb.st_mtime);

			strftime(buffer,80,"%b %d %R",timeinfo);
			fI.date_time = buffer;
			vfI.push_back(fI);
		}
		int maxlinks = *max_element(nlks.begin(),nlks.end());
		int maxsize = *max_element(fsizes.begin(),fsizes.end());
		int lnk_dig = 0;
		int sz_dig = 0;
		do {
			++lnk_dig;
			maxlinks /= 10;
		} while (maxlinks);
		do {
			++sz_dig;
			maxsize /= 10;
		} while (maxsize);
		
		cout << "total " << totalBlks/2 << endl;
		for (cnt = 0; cnt < vfI.size(); cnt++)
		{
			struct fileInfo f = vfI.at(cnt);
			if ((!a_flag) && (f.fname.at(0) == '.'))
			{
				continue;
			}
			else 
			{
				cout << f.fperm << " " << setw(lnk_dig) << right;
                cout << f.lnks << " " << f.usr << " " << f.grp << " ";
                cout << setw(sz_dig) << right << f.sz << " " << f.date_time;
                cout << " " << f.fname << endl;
            }
		}
	}
	else if(a_flag)
	{
		for (unsigned cnt = 0; cnt < f_names.size(); cnt++)
		{
			cout << f_names.at(cnt) << "  ";
		}
		cout << endl;
	}


	return 0;
}

