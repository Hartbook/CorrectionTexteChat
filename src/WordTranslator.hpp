#ifndef WORDTRANSLATOR__H
#define WORDTRANSLATOR__H

#include <vector>

class WordTranslator
{
	public :

	virtual const std::vector< std::pair<unsigned int, float> > & getTranslations(unsigned int token)=0;
};

#endif
