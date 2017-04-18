#include "Lexicon.hpp"
#include "util.hpp"

constexpr unsigned int Lexicon::unknown;
constexpr unsigned int Lexicon::mail;
constexpr unsigned int Lexicon::number;
constexpr unsigned int Lexicon::properNoun;
constexpr char Lexicon::unknownStr[];
constexpr char Lexicon::mailStr[];
constexpr char Lexicon::numberStr[];
constexpr char Lexicon::properNounStr[];

Lexicon::Lexicon()
{
	static std::unordered_map<std::string, unsigned int> specials =
	{
		{unknownStr, unknown},
		{mailStr, mail},
		{numberStr, number},
		{properNounStr, properNoun}
	};

	this->nextToken = unknown + specials.size();

	initMaps(specials);
}

void Lexicon::read(File & input)
{
	static char word[1024];
	unsigned int token;

	while (fscanf(input.getDescriptor(), "<%u>%*[^<]<%[^>]>\n", &token, word) == 2)
		addWord(word, token);
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
		if (s[i] >= 'A' && s[i] <= 'Z')
			s[i] += -'A' + 'a';
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
	for (auto it : tokens)
	{
		unsigned int padding = 50;

		fprintf(output, "<%06d>", it.second);

		padding -= 6;

		for (unsigned int i = 0; i < padding; i++)
			fprintf(output, " ");

		fprintf(output, "<%s>\n", it.first.c_str());
	}
}

