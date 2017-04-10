#ifndef FILE__H
#define FILE__H

#include <cstdio>
#include <stack>
#include <string>

class File
{
	public :

	FILE * file;
	std::stack<char> buffer;

	public :

	File(std::string & filename, std::string mode);
	~File();
	char peek();
	bool isFinished();
	char getChar();
	void ungetChar(char c);
};

#endif
