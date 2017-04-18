#ifndef TOKENIZER__H
#define TOKENIZER__H

#include "Lexicon.hpp"
#include "File.hpp"

class Tokenizer
{
	private :

	Lexicon & lexicon;

	public :

	Tokenizer(Lexicon & lexicon);
	unsigned int tokenize(File & corpus);
};

#endif

