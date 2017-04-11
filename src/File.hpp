#ifndef FILE__H
#define FILE__H

#include <cstdio>
#include <stack>
#include <string>

class File
{
	private :

	FILE * file;
	std::stack<char> buffer;
	bool endHasBeenReached;
	std::string filename;

	public :

	File(std::string & filename, std::string mode);
	~File();
	char peek();
	bool isFinished();
	char getChar();
	void ungetChar(char c);
	FILE * getDescriptor();
	std::string getName();
};

#endif
