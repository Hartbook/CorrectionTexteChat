#include "Database.hpp"
#include "File.hpp"
#include "Tokenizer.hpp"
#include "util.hpp"
#include <cstdio>
#include <memory>

Database::Database() : levenshteinTranslator(correctLexicon, incorrectLexicon)
{
}

void Database::readFromFiles(std::string incorrectFilename, std::string correctFilename,
	std::string gramsFilename, std::string tableFilename)
{
	File incorrectLexiconFile(incorrectFilename, "r");
	File correctLexiconFile(correctFilename, "r");
	File gramsCountFile(gramsFilename, "r");
	File translationTableFile(tableFilename, "r");

	correctLexicon.read(correctLexiconFile);
	incorrectLexicon.read(incorrectLexiconFile);
	gramsCounter.read(gramsCountFile);
	translationTable.read(translationTableFile);

	File test(tableFilename+".debug", "w");
	translationTable.printForDebug(test.getDescriptor(), correctLexicon, incorrectLexicon);
}

void Database::buildFromCorpus(const std::vector<std::string> & lexicons,
	const std::vector<std::string> & corpora,
	const std::vector< std::pair<std::string, std::string> > & pairs)
{
	static const char * pathToCorrectLexicon = "data/lexicon/corrige/";
	static const char * pathToIncorrectLexicon = "data/lexicon/brut/";
	static const char * pathToGramsCount = "data/gramsCount/";
	static const char * pathToTranslationTable = "data/translationTable/";

	buildLexiconFromCorpora(lexicons);
	buildGramsCounterFromCorpora(corpora);
	buildTranslationTableFromCorpora(pairs);

	std::string baseName = getFilenameFromPath(pairs[0].first);
	std::string correctLexiconName = pathToCorrectLexicon + baseName + ".lexicon";
	std::string incorrectLexiconName = pathToIncorrectLexicon + baseName + ".lexicon";
	std::string gramsCounterName = pathToGramsCount + baseName + ".grams";
	std::string translationTableName = pathToTranslationTable + baseName + ".table";

	File correctLexiconFile(correctLexiconName, "w");
	File incorrectLexiconFile(incorrectLexiconName, "w");
	File gramsCounterFile(gramsCounterName, "w");
	File translationTableFile(translationTableName, "w");

	correctLexicon.print(correctLexiconFile.getDescriptor());
	incorrectLexicon.print(incorrectLexiconFile.getDescriptor());
	gramsCounter.print(gramsCounterFile.getDescriptor());
	translationTable.print(translationTableFile.getDescriptor());
}

void Database::buildTranslationTableFromCorpora(
	const std::vector< std::pair<std::string, std::string> > & filenames)
{
	static const char * pathToTemp = "data/corpus/temp/";

	std::vector< std::pair< std::unique_ptr<File>, std::unique_ptr<File> > > files;

	for (auto & name : filenames)
		files.emplace_back(new File(name.first, "r"), new File(name.second, "r"));

	std::string baseName = getFilenameFromPath(filenames[0].first);
	std::string correctCleanedName = pathToTemp + baseName + ".clean";
	std::string incorrectCleanedName = pathToTemp + baseName + ".incorrect.clean";

	File correctCleaned(correctCleanedName, "w");
	File incorrectCleaned(incorrectCleanedName, "w");

	for (auto & file : files)
	{
		cleanCorpus(*file.first.get(), incorrectCleaned);
		cleanCorpus(*file.second.get(), correctCleaned);
	}

	correctCleaned.rewind();
	incorrectCleaned.rewind();

	Tokenizer t1(incorrectLexicon);
	Tokenizer t2(correctLexicon);

	File correctTokenized(correctCleanedName + ".tokenized", "w");
	File incorrectTokenized(incorrectCleanedName + ".tokenized", "w");

	t1.tokenize(incorrectCleaned, incorrectTokenized);
	t2.tokenize(correctCleaned, correctTokenized);

	correctTokenized.rewind();
	incorrectTokenized.rewind();

	translationTable.create(correctLexicon, incorrectLexicon,
		incorrectTokenized, correctTokenized);
}

void Database::buildGramsCounterFromCorpora(const std::vector<std::string> & filenames)
{
	std::vector< std::unique_ptr<File> > files;

	for (auto & name : filenames)
		files.emplace_back(new File(name, "r"));

	std::string word;
	bool firstWordOfSentence = true;

	for (auto & filePtr : files)
	{
		File & file = *filePtr;
		unsigned int t1 = Lexicon::unknown, t2 = Lexicon::unknown, t3 = Lexicon::unknown;

		while (!file.isFinished())
		{
			firstWordOfSentence = ignoreSeparators(file);

			unsigned int token = readWord(file, word, firstWordOfSentence);

			if (token == Lexicon::unknown)
				token = correctLexicon.getToken(word);

			t1 = t2;
			t2 = t3;
			t3 = token;

			if (t3 != Lexicon::unknown)
			{
				gramsCounter.addGram(t3);
				if (t2 != Lexicon::unknown)
				{
					gramsCounter.addGram(t2, t3);
					if (t1 != Lexicon::unknown)
						gramsCounter.addGram(t1, t2, t3);
				}
			}

			word.clear();
		}
	}
}

/*
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
	incorrectLexicon.copy(correctLexicon);
	buildLexiconFromCorpus(incorrectLexicon, *incorrect, false);

	correct->rewind();
	incorrect->rewind();

	Tokenizer correctTokenizer(correctLexicon);
	Tokenizer incorrectTokenizer(incorrectLexicon);
	File * correctTokenized = correctTokenizer.tokenize(*correct, pathToTemp + getFilenameFromPath(correctName) + ".tokenized");
	File * incorrectTokenized = incorrectTokenizer.tokenize(*incorrect, pathToTemp + getFilenameFromPath(incorrectName) + ".tokenized");

	delete correct;
	delete incorrect;

	translationTable.create(correctLexicon, incorrectLexicon, *incorrectTokenized, *correctTokenized);
	File translationTableFile(pathToTranslationTable + getFilenameFromPath(correctName) + ".table", "w");

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
*/
void Database::buildLexiconFromCorpora(const std::vector<std::string> & filenames)
{
	std::vector< std::unique_ptr<File> > files;

	for (auto & name : filenames)
		files.emplace_back(new File(name, "r"));

	std::string word;
	bool firstWordOfSentence = true;

	for (auto & filePtr : files)
	{
		File & file = *filePtr;

		while (!file.isFinished())
		{
			firstWordOfSentence = ignoreSeparators(file);
			unsigned int token = readWord(file, word, firstWordOfSentence);

			if (token == Lexicon::unknown)
				token = correctLexicon.addWord(word);

			word.clear();
		}
	}

	incorrectLexicon.copy(correctLexicon);
}

