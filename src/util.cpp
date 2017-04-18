#include "util.hpp"
#include <algorithm>
#include "Lexicon.hpp"

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

	char ridden = 0;

	while (!corpus->isFinished())
	{
		if (ridden == '\n' && corpus->peek() == '#')
			corpus->readUntil('\n');

		if (ridden == '\n' && corpus->peek() == '[')
		{
			corpus->readUntil(']');

			corpus->readUntil(':');
		}

		if (corpus->isFinished())
			break;

		fprintf(result->getDescriptor(), "%c", ridden = corpus->getChar());
	}

	delete result;

	return new File(pathName, "r");
}

unsigned int readWord(File & corpus, std::string & word, bool sentenceBegin)
{
	unsigned int indexOfFirstSeparator = 0;
	bool startsWithUpperCase = isUpper(corpus.peek());

	while (!corpus.isFinished() && !isSeparator(corpus.peek()))
	{
		word.push_back(corpus.getChar());
		indexOfFirstSeparator++;
	}

	if (corpus.peek() == ' ')
	{
		unsigned int token = Lexicon::unknown;

		if (!sentenceBegin && startsWithUpperCase)
			token = Lexicon::properNoun;

		return token;
	}

	bool containsAtSign = false;

	while (!corpus.isFinished() && corpus.peek() != ' ')
	{
		char c = corpus.getChar();

		if (c == '@')
			containsAtSign = true;

		word.push_back(c);
	}

	if (containsAtSign)
		return Lexicon::mail;

	unsigned int nbToUnget = word.size() - indexOfFirstSeparator;

	for (unsigned int i = 0; i < nbToUnget; i++)
	{
		corpus.ungetChar(word[word.size()-1]);
		word.pop_back();
	}

	unsigned int token = Lexicon::unknown;

	if (!sentenceBegin && startsWithUpperCase)
		token = Lexicon::properNoun;

	return token;
}

bool ignoreSeparators(File & corpus)
{
	bool sentenceBegin = false;

	while (!corpus.isFinished() && isSeparator(corpus.peek()))
	{
		char c = corpus.getChar();

		if (endSentence(c))
			sentenceBegin = true;
	}

	return sentenceBegin;
}

