#include "Lexicon.hpp"
#include "util.hpp"

unsigned int Lexicon::unknown = 0;

Lexicon::Lexicon()
{
	this->nextToken = 1;
}

std::string & Lexicon::normalize(std::string & s)
{
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += -'A' + 'a';
	}

	return s;
}

unsigned int Lexicon::getToken(std::string & s)
{
	s = normalize(s);

	auto it = lexicon.find(s);

	if (it == lexicon.end())
		return unknown;

	return it->second;
}

std::string Lexicon::getString(unsigned int token)
{
	for (auto it : lexicon)
		if (it.second == token)
			return it.first;

	return "UNKNOWN";
}

unsigned int Lexicon::addWord(std::string word)
{
	word = normalize(word);

	if (word.empty())
		return unknown;

	if (lexicon.count(word))
		return lexicon[word];

	return lexicon[word] = nextToken++;
}

void Lexicon::print(FILE * output)
{
	for (auto it : lexicon)
		fprintf(output, "<%d>\t\t<%s>\n", it.second, it.first.c_str());
}

