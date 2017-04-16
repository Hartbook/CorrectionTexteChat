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

	auto it = tokens.find(s);

	if (it == tokens.end())
		return unknown;

	return it->second;
}

const std::string & Lexicon::getString(unsigned int token)
{
	if (!strings.count(token))
		return *strings[unknown];

	return *strings[token];
}

unsigned int Lexicon::addWord(const std::string & word)
{
	std::string normalized = word;
	normalized = normalize(normalized);

	if (normalized.empty())
		return unknown;

	if (tokens.count(normalized))
		return tokens[normalized];

	unsigned int token = nextToken++;
	tokens[normalized] = token;
	
	const auto & it = tokens.find(normalized);
	strings[token] = &(it->first);

	return token;
}

void Lexicon::print(FILE * output)
{
	for (auto it : tokens)
		fprintf(output, "<%d>\t\t<%s>\n", it.second, it.first.c_str());
}

