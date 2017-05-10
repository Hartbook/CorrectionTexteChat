#include "Tokenizer.hpp"
#include "util.hpp"

Tokenizer::Tokenizer(Lexicon & lexicon) : lexicon(lexicon)
{
}

unsigned int Tokenizer::tokenize(File & corpus)
{
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

void Tokenizer::tokenize(File & src, File & dest)
{
	std::string word;

	unsigned int token;
	bool firstWordOfSentence = ignoreSeparators(src);

	while (!src.isFinished())
	{
		word.clear();

		if (firstWordOfSentence)
			fprintf(dest.getDescriptor(), "\n");

		token = readWord(src, word, firstWordOfSentence);

		if (token == Lexicon::unknown || token == Lexicon::properNoun)
		{
			unsigned int realToken = lexicon.getToken(word);

			if (realToken != Lexicon::unknown)
				token = realToken;
		}

		if (token == Lexicon::unknown)
			token = lexicon.addWord(word);

		fprintf(dest.getDescriptor(), "%d ", token);

		firstWordOfSentence = ignoreSeparators(src);
	}
}

