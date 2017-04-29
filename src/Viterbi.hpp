#ifndef VITERBI__H
#define VITERBI__H

#include <vector>
#include "Database.hpp"
#include "WordTranslator.hpp"

class Viterbi
{
	private :

	Database & database;
	Lexicon & lexicon;
	GramsCounter & gramsCounter;
	std::vector<WordTranslator*> translators;

	using Pair = std::pair<unsigned int, unsigned int>;
	struct Trio
	{
		Pair trad; float proba; int parent;
		Trio(Pair trad, float proba, int parent) :
			trad(trad),
			proba(proba),
			parent(parent){}
	};
	
	std::vector< std::vector<Trio> > probas;
	std::vector<unsigned int> corrected;

	private :

	void buildLatticeFromSentence(const std::vector<unsigned int> & sentence);
	void computeViterbiForRow(unsigned int row);
	void printLatticeForDebug();

	const std::vector<unsigned int> & correctSentence(
		const std::vector<unsigned int> & sentence);

	public :

	Viterbi(Database & database);
	File * correct(std::string inputFilename);
};

#endif

