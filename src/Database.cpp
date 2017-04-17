#include "Database.hpp"
#include "File.hpp"
#include "Tokenizer.hpp"
#include "util.hpp"
#include <cstdio>

void Database::buildFromCorpus(std::string correctName, std::string incorrectName)
{
	static const char * pathToCorrectLexicon = "data/lexicon/corrige/";
	static const char * pathToIncorrectLexicon = "data/lexicon/brut/";
	static const char * pathToGramsCount = "data/gramsCount/";
	static const char * pathToTemp = "data/corpus/temp/";

	File * correctUncleaned = new File(correctName, "r");
	File * incorrectUncleaned = new File(incorrectName, "r");

	File * correct = cleanCorpus(correctUncleaned, pathToTemp);
	File * incorrect = cleanCorpus(incorrectUncleaned, pathToTemp);

	delete correctUncleaned;
	delete incorrectUncleaned;

	buildLexiconFromCorpus(correctLexicon, *correct, true);
	buildLexiconFromCorpus(incorrectLexicon, *incorrect, false);

	delete correct;
	delete incorrect;

	std::string gramsFilename = pathToGramsCount + getFilenameFromPath(correctName) + ".grams";
	correctName = pathToCorrectLexicon + getFilenameFromPath(correctName) + ".lexicon";
	incorrectName = pathToIncorrectLexicon + getFilenameFromPath(incorrectName) + ".lexicon";

	correct = new File(correctName, "w");
	incorrect = new File(incorrectName, "w");
	File gramsFile(gramsFilename, "w");

	correctLexicon.print(correct->getDescriptor());
	incorrectLexicon.print(incorrect->getDescriptor());
	gramsCounter.print(gramsFile.getDescriptor(), correctLexicon);

	delete correct;
	delete incorrect;
}

void Database::buildLexiconFromCorpus(Lexicon & lexicon, File & corpus, bool countGrams)
{
	std::string word;
	bool firstWordOfSentence = true;

	auto ignoreSeparators = [&]()
	{
		while (!corpus.isFinished() && isSeparator(corpus.peek()))
		{
			char c = corpus.getChar();

			if (endSentence(c))
				firstWordOfSentence = true;
		}
	};

	auto readWord = [&]()
	{
		unsigned int indexOfFirstSeparator = 0;
		bool startsWithUpperCase = isUpper(corpus.peek());

		while (!corpus.isFinished() && !isSeparator(corpus.peek()))
		{
			word.push_back(corpus.getChar());
			indexOfFirstSeparator++;
		}

		if (corpus.peek() == ' ')
		{
			unsigned int token = Lexicon::unknown;

			if (!firstWordOfSentence && startsWithUpperCase)
				token = Lexicon::properNoun;

			firstWordOfSentence = false;

			return token;
		}

		bool containsAtSign = false;

		while (!corpus.isFinished() && corpus.peek() != ' ')
		{
			char c = corpus.getChar();

			if (c == '@')
				containsAtSign = true;

			word.push_back(c);
		}

		if (containsAtSign)
		{
			firstWordOfSentence = false;
			return Lexicon::mail;
		}

		unsigned int nbToUnget = word.size() - indexOfFirstSeparator;

		for (unsigned int i = 0; i < nbToUnget; i++)
		{
			corpus.ungetChar(word[word.size()-1]);
			word.pop_back();
		}

		unsigned int token = Lexicon::unknown;

		if (!firstWordOfSentence && startsWithUpperCase)
			token = Lexicon::properNoun;

		firstWordOfSentence = false;

		return token;
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
		unsigned int token = readWord();

		if (token == 0)
			token = lexicon.addWord(word);

		if (countGrams)
			addGrams(token);

		word.clear();
	}
}

