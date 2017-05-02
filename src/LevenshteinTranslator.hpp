#ifndef LEVENSHTEINTRANSLATOR__H
#define LEVENSHTEINTRANSLATOR__H

#include "Lexicon.hpp"
#include "WordTranslator.hpp"

class LevenshteinTranslator : public WordTranslator
{
	private :

	Lexicon & correctLexicon;
	Lexicon & incorrectLexicon;
	std::vector< std::pair<unsigned int, float> > translations;
	std::vector< std::vector<float> > distances;

	float getSubstitutionCost(const std::string & s1, const std::string & s2,
							  unsigned int i, unsigned int j);
	float getProximity(const std::string & s1, const std::string & s2);

	public :

	LevenshteinTranslator(Lexicon & correctLexicon, Lexicon & incorrectLexicon);
	void addTranslations(std::vector< std::pair<unsigned int, float> > & actual,
						 unsigned int token);
};

#endif
