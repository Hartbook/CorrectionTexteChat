#include "util.hpp"
#include <unordered_map>

bool equalsChar(char a, char b)
{
	return a == b;
}

bool isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isNum(char c)
{
	return c >= '0' && c <= '9';
}

bool isSeparator(char c)
{
	return (c == EOF) || (!isAlpha(c) && !isNum(c) && (c >= 0));
}

bool containsLetter(char * buffer)
{
	while (*buffer != '\0')
	{
		if (!isSeparator(*buffer))
			return true;

		buffer++;
	}

	return false;
}

FILE * readFile(const char * filename)
{
	FILE * file = fopen(filename, "r");

	if (!file)
	{
		printf("Can't open file %s\n", filename);
		exit(1);
	}

	return file;
}

FILE * writeFile(const char * filename)
{
	FILE * file = fopen(filename, "w");

	if (!file)
	{
		printf("Can't open file %s\n", filename);
		exit(1);
	}

	return file;
}

FILE * readFile(const std::string & filename)
{
	return readFile(filename.c_str());
}

FILE * writeFile(const std::string & filename)
{
	return writeFile(filename.c_str());
}

