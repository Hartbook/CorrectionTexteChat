#include "Database.hpp"
#include "File.hpp"
#include "Tokenizer.hpp"
#include "util.hpp"
#include "Executor.hpp"
#include <cstdio>
#include <stack>

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

	std::string baseName = getFilenameFromPath(pairs[0].first);
	std::string correctLexiconName = pathToCorrectLexicon + baseName + ".lexicon";
	std::string incorrectLexiconName = pathToIncorrectLexicon + baseName + ".lexicon";
	std::string gramsCounterName = pathToGramsCount + baseName + ".grams";
	std::string translationTableName = pathToTranslationTable + baseName + ".table";

	buildLexiconFromCorpora(lexicons);

	buildGramsCounterFromCorpora(corpora);
	File gramsCounterFile(gramsCounterName, "w");
	gramsCounter.print(gramsCounterFile.getDescriptor());

	buildTranslationTableFromCorpora(pairs);
	File translationTableFile(translationTableName, "w");
	translationTable.print(translationTableFile.getDescriptor());

	File correctLexiconFile(correctLexiconName, "w");
	correctLexicon.print(correctLexiconFile.getDescriptor());

	File incorrectLexiconFile(incorrectLexiconName, "w");
	incorrectLexicon.print(incorrectLexiconFile.getDescriptor());
}

void Database::buildTranslationTableFromCorpora(
	const std::vector< std::pair<std::string, std::string> > & filenames)
{
	static const char * pathToTemp = "data/corpus/temp/";

	using FileUptr = std::unique_ptr<File>;
	std::vector< std::pair<FileUptr,FileUptr> > files;

	for (auto & name : filenames)
	{
		std::pair<FileUptr,FileUptr>
			p(FileUptr(new File(name.first, "r")), FileUptr(new File(name.second, "r")));
		files.push_back(std::move(p));
	}

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

	t2.tokenize(correctCleaned, correctTokenized);

	incorrectLexicon.copy(correctLexicon);

	t1.tokenize(incorrectCleaned, incorrectTokenized);

	correctTokenized.rewind();
	incorrectTokenized.rewind();

	translationTable.create(correctLexicon, incorrectLexicon,
		incorrectTokenized, correctTokenized);
}

void Database::buildGramsCounterFromCorpora(const std::vector<std::string> & filenames)
{
	static unsigned int linesPerSplit = 1000 * 250;
	static const char * pathToTemp = "data/corpus/temp/";

	std::stack< std::unique_ptr<File> > inputFiles;
	std::vector< std::unique_ptr<File> > splittedFiles;

	for (auto & name : filenames)
		inputFiles.emplace(new File(name, "r"));

	unsigned int linesCount = 0;
	char buffer[10000];

	while (!inputFiles.empty())
	{
		File & inputFile = *inputFiles.top().get();

		while (!inputFile.isFinished())
		{
			if (linesCount % linesPerSplit == 0)
				splittedFiles.emplace_back(new File(pathToTemp+std::to_string(linesCount), "w"));

			FILE * destDesc = splittedFiles.back().get()->getDescriptor();

			if (fscanf(inputFile.getDescriptor(), "%[^\n]\n", buffer) != 1)
				break;

			fprintf(destDesc, "%s\n", buffer);

			linesCount++;
		}

		inputFiles.pop();
	}

	Executor<void,void> executor;

	std::vector< std::vector<unsigned int> > tokenized;
	tokenized.resize(splittedFiles.size());

	for (unsigned int fileIndex = 0; fileIndex < splittedFiles.size(); fileIndex++)
	{
		File * filePtr = splittedFiles[fileIndex].get();
		std::vector<unsigned int> * row = &tokenized[fileIndex];

		executor.addTask([row,filePtr,this]()
		{
			std::string word;
			bool firstWordOfSentence = true;
			File & file = *filePtr;

			file.rewind();

			while (!file.isFinished())
			{
				firstWordOfSentence = ignoreSeparators(file);

				if (firstWordOfSentence)
					row->emplace_back(Lexicon::newSentence);

				unsigned int token = readWord(file, word, firstWordOfSentence);

				if (token == Lexicon::unknown)
					token = correctLexicon.getToken(word);

				row->emplace_back(token);

				word.clear();
			}
		});
	}

	executor.run();

	for (auto & row : tokenized)
	{
		unsigned int t1 = Lexicon::unknown, t2 = Lexicon::unknown, t3 = Lexicon::unknown;

		for (unsigned int i = 0; i < row.size(); i++)
		{
			t1 = t2;
			t2 = t3;
			t3 = row[i];

			if (t3 == Lexicon::newSentence)
			{
				t1 = Lexicon::unknown;
				t2 = Lexicon::unknown;
				t3 = Lexicon::unknown;
			}

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
		}
	}
}

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
}

