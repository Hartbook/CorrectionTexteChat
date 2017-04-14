#include "util.hpp"
#include <algorithm>

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
	if (c == '-')
		return false;

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

std::string getFilenameFromPath(std::string s)
{
	std::string result;

	while (!s.empty() && s[s.size()-1] != '/')
	{
		result.push_back(s[s.size()-1]);
		s.pop_back();
	}

	std::reverse(result.begin(), result.end());

	return result;
}

