#ifndef VITERBI__H
#define VITERBI__H

#include <vector>
#include "Database.hpp"
#include "WordTranslator.hpp"

class Viterbi
{
	private :

	Database & database;
	std::vector<WordTranslator*> translators;

	const std::vector<unsigned int> & correctSentence(
		const std::vector<unsigned int> & sentence);

	public :

	Viterbi(Database & database);
	File * correct(std::string inputFilename);
};

#endif

