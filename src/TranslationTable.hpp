#ifndef TRANSLATIONTABLE__H
#define TRANSLATIONTABLE__H

#include <set>
#include "File.hpp"
#include "Lexicon.hpp"
#include "AtomicFloat.hpp"
#include "WordTranslator.hpp"

class TranslationTable : public WordTranslator
{
	private :

	static constexpr int nbIterations = 10;
	static float minimalProb;

	std::vector< std::vector<AtomicFloat> > table;

	unsigned int maxTokenCorrect;

	bool isCorrect(unsigned int token);

//	std::vector< std::pair<unsigned int, float> > translations;

	public :

	TranslationTable();
	void create(const Lexicon & correctLexicon, const Lexicon & incorrectLexicon,
		File & incorrect, File & correct);
	void print(FILE * output);
	void printForDebug(FILE * output, Lexicon & correctLex,
		Lexicon & incorrectLex);
	void read(File & input);
	void addTranslations(std::vector< std::pair<unsigned int, float> > & actual,
						 unsigned int token);
};

#endif
