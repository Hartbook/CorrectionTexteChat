#ifndef WORDTRANSLATOR__H
#define WORDTRANSLATOR__H

#include <vector>

class WordTranslator
{
	public :

	unsigned int order;

	virtual void addTranslations(std::vector< std::pair<unsigned int, float> > & actual,
								 unsigned int token) = 0;
};

#endif

