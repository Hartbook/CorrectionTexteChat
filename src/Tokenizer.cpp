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

unsigned int Tokenizer::tokenize(File & corpus)
{
	static auto ignoreSeparators = [&]() {
		while (!corpus.isFinished() && isSeparator(corpus.peek()))
			corpus.getChar();
	};

	static auto putBackCharacters = [&]() {
		char * charToPutBack = trie.getCharToPutBack();
		int nbCharToPutBack = trie.getNbCharToPutBack();

		for (int i = nbCharToPutBack-1; i >= 0; i--)
			corpus.ungetChar(charToPutBack[i]);
	};

	int code = -1;

	ignoreSeparators();

	while (!corpus.isFinished() && code != 0)
	{
		char character = corpus.getChar();
		code = trie.processChar(character);
	}

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

