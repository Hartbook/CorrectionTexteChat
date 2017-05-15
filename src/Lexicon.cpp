#include "Lexicon.hpp"
#include <vector>
#include <algorithm>
#include <cassert>
#include "util.hpp"

constexpr unsigned int Lexicon::unknown;
constexpr unsigned int Lexicon::mail;
constexpr unsigned int Lexicon::number;
constexpr unsigned int Lexicon::properNoun;
constexpr unsigned int Lexicon::date;
constexpr unsigned int Lexicon::url;
constexpr unsigned int Lexicon::newSentence;
constexpr char Lexicon::unknownStr[];
constexpr char Lexicon::mailStr[];
constexpr char Lexicon::numberStr[];
constexpr char Lexicon::properNounStr[];
constexpr char Lexicon::dateStr[];
constexpr char Lexicon::urlStr[];
constexpr char Lexicon::newSentenceStr[];

Lexicon::Lexicon()
{
	static std::unordered_map<std::string, unsigned int> specials =
	{
		{unknownStr, unknown},
		{mailStr, mail},
		{numberStr, number},
		{properNounStr, properNoun},
		{dateStr, date},
		{urlStr, url}
	};

	this->nextToken = unknown + specials.size();

	initMaps(specials);
}

void Lexicon::read(File & input)
{
	static char word[1024];
	unsigned int token;

	while (fscanf(input.getDescriptor(), "<%u>%*[^<]<%[^>]>\n", &token, word) == 2)
	{
		addWord(word, token);
		nextToken = std::max(nextToken, token);
	}

	nextToken++;
}

void Lexicon::initMaps(std::unordered_map<std::string, unsigned int> & specials)
{
	for (auto & it : specials)
	{
		tokens[it.first] = it.second;
		strings[it.second] = &it.first;
	}
}

std::string & Lexicon::normalize(std::string & s) const
{
	for (unsigned int i = 0; i < s.size(); i++)
		toLowerCase(s, i);

	return s;
}

unsigned int Lexicon::getToken(std::string & s) const
{
	if (s != unknownStr && s != mailStr && s != numberStr && s != properNounStr && s != dateStr)
		s = normalize(s);

	if (isNum(s))
		return number;

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

	if (isNum(normalized))
		return number;

	if (tokens.count(normalized))
		return tokens[normalized];

	unsigned int token = nextToken++;
	tokens[normalized] = token;
	
	const auto & it = tokens.find(normalized);
	strings[token] = &(it->first);

	return token;
}

void Lexicon::addWord(std::string word, unsigned int token)
{
	tokens[word] = token;
	const auto & it = tokens.find(word);
	strings[token] = &(it->first);
}

void Lexicon::print(FILE * output)
{
	for (unsigned int token = 0; token < nextToken; token++)
	{
		unsigned int padding = 50;

		fprintf(output, "<%06d>", token);

		padding -= 6;

		for (unsigned int i = 0; i < padding; i++)
			fprintf(output, " ");

		fprintf(output, "<%s>\n", getString(token).c_str());
	}
}

void Lexicon::copy(const Lexicon & other)
{
	nextToken = 0;
	tokens.clear();	
	strings.clear();

	for (auto & it : other.tokens)
	{
		addWord(it.first, it.second);
		nextToken = std::max(nextToken, it.second);
	}

	nextToken++;
}

unsigned int Lexicon::getMaxToken() const
{
	return nextToken-1;
}

const std::unordered_map<std::string, unsigned int> & Lexicon::getTokens()
{
	return tokens;
}

