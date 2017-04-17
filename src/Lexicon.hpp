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

	static constexpr unsigned int unknown = 0;
	static constexpr char unknownStr[] = "UNKNOWN";

	static constexpr unsigned int mail = 1;
	static constexpr char mailStr[] = "MAIL";

	static constexpr unsigned int number = 2;
	static constexpr char numberStr[] = "NUMBER";

	static constexpr unsigned int properNoun = 3;
	static constexpr char properNounStr[] = "PROPERNOUN";

	Lexicon();
	void initMaps(std::unordered_map<std::string, unsigned int> & specials);
	unsigned int getToken(std::string & s);
	const std::string & getString(unsigned int token);
	unsigned int addWord(const std::string & word);
	void print(FILE * output);
};

#endif
