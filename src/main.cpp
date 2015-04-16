#include <string.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

using namespace std;

int check_connect(const string& str, int& pos, int start){
	unsigned found = 0;
	found = str.find_first_of(";&|#",start);
	if (found >= str.size()){
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
	int start = 0;
	int c_stat;
	int pos = -1;
	vector<string> str;
	str.push_back("The first string, no connectors");
	str.push_back("Second string; Semicolon included");
	str.push_back("&Third: && got that ampersand");
	str.push_back("We has | in i||t");
	str.push_back("This has a #comment");
	cout << "entering for loop" << endl;
	for (int i = 0; i < 5; ++i){
		c_stat = check_connect(str.at(i), pos, start);
		cout << "string " << i << ": " << c_stat << " at: " << pos << endl;		
	}
	return 0;
}


