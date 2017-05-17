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
	bool firstWordOfSentence;
	bool firstTime = true;

	while (!src.isFinished())
	{
		word.clear();

		firstWordOfSentence = ignoreSeparators(src);
		if (firstTime)
		{
			firstWordOfSentence = true;
			firstTime = false;
		}

		if (firstWordOfSentence)
			fprintf(dest.getDescriptor(), "\n");

		token = readWord(src, word, firstWordOfSentence);

		if (word.empty())
			break;

		if (token == Lexicon::unknown || token == Lexicon::properNoun)
		{
			unsigned int realToken = lexicon.getToken(word);

			if (realToken != Lexicon::unknown)
				token = realToken;
		}

		if (token == Lexicon::unknown)
		{
			bool containsApostrophe = false;
			for (unsigned int i = 0; i < word.size(); i++)
			{
				if (word[i] == '\'')
				{
					containsApostrophe = true;
					word[i] = ' ';

					while (!word.empty())
					{
						src.ungetChar(word.back());
						word.pop_back();
					}

					break;
				}
			}

			if (containsApostrophe)
				continue;

			token = lexicon.addWord(word);
		}

		fprintf(dest.getDescriptor(), "%d ", token);
	}
}

