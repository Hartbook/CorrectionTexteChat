#include "Database.hpp"
#include "File.hpp"
#include "Tokenizer.hpp"
#include "util.hpp"
#include <cstdio>

void Database::readFromFiles(std::string incorrectFilename, std::string correctFilename,
	std::string gramsFilename, std::string tableFilename)
{
	File incorrectLexiconFile(incorrectFilename, "r");
	File correctLexiconFile(correctFilename, "r");
	File gramsCountFile(gramsFilename, "r");

	correctLexicon.read(correctLexiconFile);
	incorrectLexicon.read(incorrectLexiconFile);
	gramsCounter.read(gramsCountFile);
}

void Database::buildFromCorpus(std::string correctName, std::string incorrectName)
{
	static const char * pathToCorrectLexicon = "data/lexicon/corrige/";
	static const char * pathToIncorrectLexicon = "data/lexicon/brut/";
	static const char * pathToGramsCount = "data/gramsCount/";
	static const char * pathToTranslationTable = "data/translationTable/";
	static const char * pathToTemp = "data/corpus/temp/";

	File * correctUncleaned = new File(correctName, "r");
	File * incorrectUncleaned = new File(incorrectName, "r");

	File * correct = cleanCorpus(correctUncleaned, pathToTemp);
	File * incorrect = cleanCorpus(incorrectUncleaned, pathToTemp);

	delete correctUncleaned;
	delete incorrectUncleaned;

	buildLexiconFromCorpus(correctLexicon, *correct, true);
	buildLexiconFromCorpus(incorrectLexicon, *incorrect, false);

	correct->rewind();
	incorrect->rewind();

	Tokenizer correctTokenizer(correctLexicon);
	Tokenizer incorrectTokenizer(incorrectLexicon);
	File * correctTokenized = correctTokenizer.tokenize(*correct, pathToTemp + getFilenameFromPath(correctName) + ".tokenized");
	File * incorrectTokenized = incorrectTokenizer.tokenize(*incorrect, pathToTemp + getFilenameFromPath(incorrectName) + ".tokenized");

	delete correct;
	delete incorrect;

	translationTable.create(*incorrectTokenized, *correctTokenized);
	File translationTableFile(pathToTranslationTable + getFilenameFromPath(correctName) + ".table", "w");

	translationTable.printMostProbableTranslation(stdout, correctLexicon, incorrectLexicon);
	translationTable.print(translationTableFile.getDescriptor());

	delete correctTokenized;
	delete incorrectTokenized;

	std::string gramsFilename = pathToGramsCount + getFilenameFromPath(correctName) + ".grams";
	correctName = pathToCorrectLexicon + getFilenameFromPath(correctName) + ".lexicon";
	incorrectName = pathToIncorrectLexicon + getFilenameFromPath(incorrectName) + ".lexicon";
	
	correct = new File(correctName, "w");
	incorrect = new File(incorrectName, "w");
	File gramsFile(gramsFilename, "w");

	correctLexicon.print(correct->getDescriptor());
	incorrectLexicon.print(incorrect->getDescriptor());
	gramsCounter.print(gramsFile.getDescriptor());

	delete correct;
	delete incorrect;
}

void Database::buildLexiconFromCorpus(Lexicon & lexicon, File & corpus, bool countGrams)
{
	std::string word;
	bool firstWordOfSentence = true;

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
		firstWordOfSentence = ignoreSeparators(corpus);
		unsigned int token = readWord(corpus, word, firstWordOfSentence);

		if (token == 0)
			token = lexicon.addWord(word);

		if (countGrams)
			addGrams(token);

		word.clear();
	}
}

