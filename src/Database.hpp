#ifndef DATABASE__H
#define DATABASE__H

#include <string>
#include "Lexicon.hpp"
#include "GramsCounter.hpp"
#include "TranslationTable.hpp"

class Database
{
	private :

	Lexicon correctLexicon;
	Lexicon incorrectLexicon;
	GramsCounter gramsCounter;
	TranslationTable translationTable;

	void buildLexiconFromCorpus(Lexicon & lexicon, File & corpus, bool countGrams);

	public :

	void buildFromCorpus(std::string correctName, std::string incorrectName);
	void readFromFiles(std::string incorrectFilename, std::string correctFilename,
		std::string gramsFilename, std::string tableFilename);
};

#endif
