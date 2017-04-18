#include "Tokenizer.hpp"
#include "util.hpp"

Tokenizer::Tokenizer(Lexicon & lexicon) : lexicon(lexicon)
{
}

unsigned int Tokenizer::tokenize(File & corpus)
{
	static std::string word;
	word.clear();

	bool firstWordOfSentence = ignoreSeparators(corpus);
	unsigned int token = readWord(corpus, word, firstWordOfSentence);

	if (token == Lexicon::unknown || token == Lexicon::properNoun)
	{
		unsigned int realToken = lexicon.getToken(word);

		if (realToken != Lexicon::unknown)
			token = realToken;
	}

	return token;
}

