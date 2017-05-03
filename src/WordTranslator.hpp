#ifndef WORDTRANSLATOR__H
#define WORDTRANSLATOR__H

#include <vector>
#include <unordered_map>
#include <memory>

class WordTranslations
{
	public :

	using Pair = std::pair<unsigned int, float>;

	unsigned int token;
	std::vector< std::unique_ptr<Pair> > translations;
	std::unordered_map<unsigned int, Pair *> positions;

	public :

	float combineProbas(float p1, float p2);
	void initForToken(unsigned int token);
	void addTranslation(unsigned int token, float proba);
	bool empty();
};

class WordTranslator
{
	public :

	unsigned int order;

	virtual void addTranslations(WordTranslations & actual) = 0;
};

#endif

