#ifndef LEXICON__H
#define LEXICON__H

#include <string>
#include <unordered_map>
#include "File.hpp"

class Lexicon
{
	private :

	std::unordered_map<std::string, unsigned int> tokens;
	std::unordered_map<unsigned int, const std::string *> strings;
	unsigned int nextToken;

	std::string & normalize(std::string & s);

	public :

	static unsigned int unknown;
	static std::string unknownStr;

	Lexicon();
	unsigned int getToken(std::string & s);
	const std::string & getString(unsigned int token);
	unsigned int addWord(const std::string & word);
	void print(FILE * output);
};

#endif
