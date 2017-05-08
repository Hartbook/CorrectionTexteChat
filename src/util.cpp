#include "util.hpp"
#include <algorithm>
#include "Lexicon.hpp"

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
		if (isAlpha(c) || c < 0)
			return false;

	return true;
}

bool isSeparator(char c)
{
	if (c == '-')
		return false;

	return (c == EOF) || (!isAlpha(c) && !isNum(c) && (c >= 0));
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
	return c == '.' || c == '!' || c == '?' || c == '\n' || c == EOF;
}

File * cleanCorpus(File * corpus, std::string path)
{
	auto isNewline = [](char c) {return c == 10 || c == 13;};
	auto isNotNewline = [](char c) {return c != 10 && c != 13;};
	auto skipNewlines = [&]()
	{
		char c = corpus->readUntil(isNotNewline);
		if (!isNewline(c))
			corpus->ungetChar(c);
	};
	auto skipControlCharacters = [&]()
	{
		while (((unsigned char)corpus->peek()) == 194)
		{
			corpus->getChar();
			corpus->getChar();
		}
	};

	std::string pathName = path + getFilenameFromPath(corpus->getName());

	File * result = new File(pathName, "w");

	char read = '\n';

	while (!corpus->isFinished())
	{
		if (isNewline(read))
			skipNewlines();

		bool triggered = true;

		while (triggered)
		{
			triggered = false;

			skipControlCharacters();

			while (isNewline(read) && corpus->peek() == '#')
			{
				triggered = true;

				read = corpus->readUntil(isNewline);

				if (isNewline(read))
					skipNewlines();
			}

			while (isNewline(read) && corpus->peek() == '[')
			{
				triggered = true;

				read = corpus->readUntil(']');

				read = corpus->readUntil([&](char c)
				{
					return isNewline(c) || c == ':';
				});

				if (read == ':')
					fprintf(result->getDescriptor(), "\n");

				if (corpus->peek() == ' ')
					read = corpus->getChar();

				if (isNewline(read))
					skipNewlines();
			}

			if (corpus->isFinished())
				break;
		}

		read = corpus->getChar();

		if (corpus->isFinished())
			break;

		if (isNewline(read))
			read = '\n';

		if (!isNewline(read))
			fprintf(result->getDescriptor(), "%c", read);
	}

	fprintf(result->getDescriptor(), "\n");

	delete result;

	return new File(pathName, "r");
}

unsigned int readWord(File & corpus, std::string & word, bool sentenceBegin)
{
	unsigned int indexOfFirstSeparator = 0;
	bool startsWithUpperCase = isUpper(corpus.peek());

	auto checkForPattern = [&]()
	{
		bool containsLetter = false;
		bool containsDigit = false;

		for (char c : word)
		{
			if (isNum(c))
				containsDigit = true;
			if (isAlpha(c))
				containsLetter = true;
		}

		if (containsLetter && containsDigit)
			return Lexicon::date;

		if (!sentenceBegin && startsWithUpperCase)
			return Lexicon::properNoun;

		return Lexicon::unknown;
	};

	auto checkForMailAddress = [&]()
	{
		bool containsAtSign = false;

		while (!corpus.isFinished() && corpus.peek() != ' ')
		{
			char c = corpus.getChar();

			if (c == '@')
				containsAtSign = true;

			word.push_back(c);
		}

		if (containsAtSign)
			return true;

		unsigned int nbToUnget = word.size() - indexOfFirstSeparator;

		for (unsigned int i = 0; i < nbToUnget; i++)
		{
			corpus.ungetChar(word[word.size()-1]);
			word.pop_back();
		}

		return false;
	};

	auto removeEndingDashes = [&]()
	{
		while (!word.empty() && word.back() == '-')
			word.pop_back();
	};

	while (!corpus.isFinished() && !isSeparator(corpus.peek()))
	{
		word.push_back(corpus.getChar());
		indexOfFirstSeparator++;
	}

	removeEndingDashes();

	if (corpus.peek() == ' ')
		return checkForPattern();

	if (checkForMailAddress())
		return Lexicon::mail;

	return checkForPattern();
}

bool ignoreSeparators(File & corpus)
{
	static auto isToBeIgnored = [](char c)
	{
		return isSeparator(c) || c == '-';
	};

	bool sentenceBegin = false;

	while (!corpus.isFinished() && isToBeIgnored(corpus.peek()))
	{
		char c = corpus.getChar();

		if (endSentence(c))
			sentenceBegin = true;
	}

	return sentenceBegin;
}

