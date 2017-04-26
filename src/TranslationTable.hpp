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
	static constexpr float minimalProb = 0.1;

	std::vector< std::vector<AtomicFloat> > table;

	unsigned int maxTokenCorrect;

	bool isCorrect(unsigned int token);

	public :

	void create(const Lexicon & correctLexicon, const Lexicon & incorrectLexicon,
		File & incorrect, File & correct);
	void print(FILE * output);
	void printForDebug(FILE * output, Lexicon & correctLex,
		Lexicon & incorrectLex);
	void read(File & input);
	std::vector< std::pair<unsigned int, float> > * getTranslations(unsigned int token);
};

#endif
