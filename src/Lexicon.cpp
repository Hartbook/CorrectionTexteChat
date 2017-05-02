#include "Lexicon.hpp"
#include <vector>
#include <algorithm>
#include "util.hpp"

constexpr unsigned int Lexicon::unknown;
constexpr unsigned int Lexicon::mail;
constexpr unsigned int Lexicon::number;
constexpr unsigned int Lexicon::properNoun;
constexpr unsigned int Lexicon::date;
constexpr char Lexicon::unknownStr[];
constexpr char Lexicon::mailStr[];
constexpr char Lexicon::numberStr[];
constexpr char Lexicon::properNounStr[];
constexpr char Lexicon::dateStr[];

Lexicon::Lexicon()
{
	static std::unordered_map<std::string, unsigned int> specials =
	{
		{unknownStr, unknown},
		{mailStr, mail},
		{numberStr, number},
		{properNounStr, properNoun},
		{dateStr, date}
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

std::string & Lexicon::normalize(std::string & s)
{
	for (unsigned int i = 0; i < s.size(); i++)
	{
		// Uncapitalize basic letters
		if (s[i] >= 'A' && s[i] <= 'Z')
		{
			s[i] += -'A' + 'a';
			continue;
		}

		// Uncapitalize accentuated letters
		if (i == s.size()-1)
			continue;
		unsigned char s1 = s[i], s2 = s[i+1];
		if (s1 == 195 && s2 >= 128 && s2 <= 158)
		{
			s2 += 32;
			s[i+1] = s2;
			continue;
		}
	}

	return s;
}

unsigned int Lexicon::getToken(std::string & s)
{
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

		if (it.second > nextToken)
			nextToken = it.second;
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

