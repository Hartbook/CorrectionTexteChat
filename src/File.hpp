#ifndef FILE__H
#define FILE__H

#include <cstdio>
#include <stack>
#include <string>
#include <functional>

class File
{
	private :

	FILE * file;
	std::stack<char> buffer;
	bool endHasBeenReached;
	std::string filename;

	public :

	File(const std::string & filename, const std::string & mode);
	~File();
	char peek();
	bool isFinished();
	char getChar();
	void ungetChar(char c);
	FILE * getDescriptor();
	const std::string & getName();
	char readUntil(char c);
	char readUntil(const std::function<bool(char)> & condition);
	void rewind();
};

#endif
