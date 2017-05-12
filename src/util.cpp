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

bool isNewline(char c)
{
	return c == 10 || c == 13;
}

void cleanCorpus(File & source, File & dest)
{
	auto isNotNewline = [](char c) {return c != 10 && c != 13;};
	auto skipNewlines = [&]()
	{
		char c = source.readUntil(isNotNewline);
		if (!isNewline(c))
			source.ungetChar(c);
	};
	auto skipControlCharacters = [&]()
	{
		while (((unsigned char)source.peek()) == 194)
		{
			source.getChar();
			source.getChar();
		}
	};

	char read = '\n';

	while (!source.isFinished())
	{
		if (isNewline(read))
			skipNewlines();

		bool triggered = true;

		while (triggered)
		{
			triggered = false;

			skipControlCharacters();

			while (isNewline(read) && source.peek() == '#')
			{
				triggered = true;

				read = source.readUntil(isNewline);

				if (isNewline(read))
					skipNewlines();
			}

			while (isNewline(read) && source.peek() == '[')
			{
				triggered = true;

				read = source.readUntil(']');

				read = source.readUntil([&](char c)
				{
					return isNewline(c) || c == ':';
				});

				if (read == ':')
					fprintf(dest.getDescriptor(), "\n");

				if (source.peek() == ' ')
					read = source.getChar();

				if (isNewline(read))
					skipNewlines();
			}

			if (source.isFinished())
				break;
		}

		read = source.getChar();

		if (source.isFinished())
			break;

		if (isNewline(read))
			read = '\n';

		if (!isNewline(read))
			fprintf(dest.getDescriptor(), "%c", read);
	}

	fprintf(dest.getDescriptor(), "\n");
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

		while (!corpus.isFinished() && corpus.peek() != ' ' && !isNewline(corpus.peek()))
		{
			char c = corpus.getChar();

			if (c == '@')
				containsAtSign = true;

			word.push_back(c);
		}

		if (containsAtSign)
			return true;

		unsigned int nbToUnget = word.size() - indexOfFirstSeparator;

		if (nbToUnget >= word.size())
			printf("toUnget = %d, size = %d\n", nbToUnget, word.size());

		for (unsigned int i = 0; i < nbToUnget; i++)
		{
			if (word.empty())
				printf("toUnget = %d, size = %d\n", nbToUnget, word.size());
			corpus.ungetChar(word.back());
			word.pop_back();
		}

		return false;
	};

	auto removeEndingDashes = [&]()
	{
		while (!word.empty() && word.back() == '-')
		{
			word.pop_back();
			indexOfFirstSeparator = std::min(indexOfFirstSeparator, word.size());
		}
	};

	while (!corpus.isFinished() && !isSeparator(corpus.peek()))
	{
		word.push_back(corpus.getChar());
		indexOfFirstSeparator++;
	}

	if (word.empty())
		return Lexicon::unknown;

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

void toLowerCase(std::string & s, unsigned int i)
{
	// Uncapitalize basic letters
	if (s[i] >= 'A' && s[i] <= 'Z')
	{
		s[i] += -'A' + 'a';
		return;
	}

	// Uncapitalize accentuated letters
	if (i == s.size()-1)
		return;
	unsigned char s1 = s[i], s2 = s[i+1];
	if (s1 == 195 && s2 >= 128 && s2 <= 158)
	{
		s2 += 32;
		s[i+1] = s2;
		return;
	}
}

void toUpperCase(std::string & s, unsigned int i)
{
	// Cpitalize basic letters
	if (s[i] >= 'a' && s[i] <= 'z')
	{
		s[i] += 'A' - 'a';
		return;
	}
}

