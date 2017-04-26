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

File * Tokenizer::tokenize(File & corpus, const std::string & path)
{
	File * file = new File(path, "w");

	std::string word;

	unsigned int token;
	bool firstWordOfSentence = ignoreSeparators(corpus);

	while (!corpus.isFinished())
	{
		word.clear();

		if (firstWordOfSentence)
			fprintf(file->getDescriptor(), "\n");

		token = readWord(corpus, word, firstWordOfSentence);

		if (token == Lexicon::unknown || token == Lexicon::properNoun)
		{
			unsigned int realToken = lexicon.getToken(word);

			if (realToken != Lexicon::unknown)
				token = realToken;
		}

		if (token == Lexicon::unknown)
			token = lexicon.addWord(word);

		fprintf(file->getDescriptor(), "%d ", token);

		firstWordOfSentence = ignoreSeparators(corpus);
	}

	delete file;

	return new File(path, "r");
}

