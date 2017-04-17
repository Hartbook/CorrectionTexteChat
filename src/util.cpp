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

bool isNum(const std::string & s)
{
	for (char c : s)
		if (isAlpha(c))
			return false;

	return true;
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

unsigned int lengthPrinted(const std::string & s)
{
	float l = 0.0;

	for (char c : s)
	{
		if (c > 0)
			l += 1;
		if (c < 0)
			l += 0.5;
	}

	return (l - (int)l > 0) ? (unsigned int) l + 1 : (unsigned int) l;
}

bool isUpper(char c)
{
	return c >= 'A' && c <= 'Z';
}

bool endSentence(char c)
{
	return c == '.' || c == '!' || c == '?' || c == '\n';
}

File * cleanCorpus(File * corpus, std::string path)
{
	std::string pathName = path + getFilenameFromPath(corpus->getName());

	File * result = new File(pathName, "w");

	while (!corpus->isFinished())
		fprintf(result->getDescriptor(), "%c", corpus->getChar());

	delete result;

	return new File(pathName, "r");
}

