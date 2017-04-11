#ifndef LEXICON__H
#define LEXICON__H

#include <string>
#include <unordered_map>
#include "File.hpp"

class Lexicon
{
	private :

	std::unordered_map<std::string, unsigned int> lexicon;
	unsigned int nextToken;

	std::string & normalize(std::string & s);

	public :

	static unsigned int unknown;

	Lexicon();
	unsigned int getToken(std::string & s);
	std::string getString(unsigned int token);
	unsigned int addWord(std::string word);
	void print(FILE * output);
};

#endif
