#ifndef TOKENIZER__H
#define TOKENIZER__H

#include <cstdio>
#include "Trie.hpp"

class Tokenizer
{
	private :

	Trie trie;
	std::string currentWord;

	public :

	Tokenizer(bool printUnknownWords = false);
	void readLexicon(FILE * lexicon);
	unsigned int tokenize(FILE * corpus);
	std::string getCurrentWord();
};

#endif

