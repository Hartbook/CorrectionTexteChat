#ifndef DATABASE__H
#define DATABASE__H

#include <string>
#include "Lexicon.hpp"
#include "GramsCounter.hpp"

class Database
{
	private :

	Lexicon correctLexicon;
	Lexicon incorrectLexicon;
	GramsCounter gramsCounter;

	void buildLexiconFromCorpus(Lexicon & lexicon, File & corpus, bool countGrams);

	public :

	void buildFromCorpus(std::string correctName, std::string incorrectName);
};

#endif
