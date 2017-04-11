#include "Database.hpp"
#include "File.hpp"
#include "Tokenizer.hpp"
#include "util.hpp"
#include <cstdio>

void Database::buildFromCorpus(std::string correctName, std::string incorrectName)
{
	File correct(correctName, "r");
	File incorrect(incorrectName, "r");

	buildLexiconFromCorpus(correctLexicon, correct, true);
	buildLexiconFromCorpus(incorrectLexicon, incorrect, false);
}

void Database::buildLexiconFromCorpus(Lexicon & lexicon, File & corpus, bool countGrams)
{
	std::string word;

	auto ignoreSeparators = [&]()
	{
		while (!corpus.isFinished() && isSeparator(corpus.peek()))
			corpus.getChar();
	};

	auto readWord = [&]()
	{
		while (!corpus.isFinished() && !isSeparator(corpus.peek()))
			word.push_back(corpus.getChar());
	};

	auto addGrams = [&](unsigned int token)
	{
		static unsigned int t1 = 0;
		static unsigned int t2 = 0;
		static unsigned int t3 = 0;

		t1 = t2;
		t2 = t3;
		t3 = token;

		if (!t3)
			return;
		gramsCounter.addGram(t3);
		if (!t2)
			return;
		gramsCounter.addGram(t2, t3);
		if (!t1)
			return;
		gramsCounter.addGram(t1, t2, t3);
	};

	while (!corpus.isFinished())
	{
		ignoreSeparators();
		readWord();
		unsigned int token = lexicon.addWord(word);

		if (countGrams)
			addGrams(token);

		word.clear();
	}

	if (countGrams)
	{
		lexicon.print(stdout);
		gramsCounter.print(stdout);
	}
}

