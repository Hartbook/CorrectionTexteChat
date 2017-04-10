#ifndef LEXICON__H
#define LEXICON__H

#include <string>
#include <unordered_map>

class Lexicon
{
	private :

	std::string filename;
	std::unordered_map<std::string, unsigned int> lexicon;
	unsigned int nextToken;

	std::string & normalize(std::string & s);

	public :

	static unsigned int unknown;

	Lexicon();
	unsigned int getToken(std::string & s);
	std::string getString(unsigned int token);
	void addWord(std::string word);
};

#endif
