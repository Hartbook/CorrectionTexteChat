#ifndef TOKENIZER__H
#define TOKENIZER__H

#include "Lexicon.hpp"
#include "File.hpp"

class Tokenizer
{
	private :

	Lexicon & lexicon;

	public :

	std::string word;

	Tokenizer(Lexicon & lexicon);
	unsigned int tokenize(File & corpus);
	File * tokenize(File & corpus, const std::string & path);
};

#endif

