#include "Tokenizer.hpp"
#include "util.hpp"
#include <cassert>

Tokenizer::Tokenizer(bool printUnknownWords) : trie(printUnknownWords)
{
}

void Tokenizer::readLexicon(FILE * lexicon)
{
	char buffer[128];
	int code;

	while(fscanf(lexicon, "%s", buffer) == 1)
	{
		fscanf(lexicon, "%d\n", &code);
		trie.addWord(buffer, code);
	}
}

unsigned int Tokenizer::tokenize(FILE * corpus)
{
	static auto readUntilSeparator = [&]() {
		char character;

		while (!isSeparator(character = fgetc(corpus)))
			trie.addToBuffer(character);

		ungetc(character, corpus);
	};

	static auto putBackCharacters = [&]() {
		char * charToPutBack = trie.getCharToPutBack();
		int nbCharToPutBack = trie.getNbCharToPutBack();

		for (int i = nbCharToPutBack-1; i >= 0; i--)
			ungetc(charToPutBack[i], corpus);
	};

	char character;
	int code = 1;

	while ((character = fgetc(corpus)) != EOF && code != 0)
		code = trie.processChar(character);

	if (trie.getNbBackups())
		do
		{
			trie.regressState();
			putBackCharacters();
		} while (trie.getNbBackups() && !trie.getCurrentCode());

	code = trie.getCurrentCode();
	currentWord = trie.getBuffer();

	trie.resetState();

	return code;
}

std::string Tokenizer::getCurrentWord()
{
	return currentWord;
}

