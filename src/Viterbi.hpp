/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef VITERBI__H
#define VITERBI__H

#include <vector>
#include "Database.hpp"
#include "WordTranslator.hpp"

/////////////////////////////////////////////////////////////////////////////
/// \brief Used to correct a File, using Viterbi algorithm.
///
////////////////////////////////////////////////////////////////////////////
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
	
	private :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Build a lattice representing every probable correction of the sentence.
	///
	/// The constructed lattice is stored into `probas`
	///
	/// \param probas Lattice to construct into.
	/// \param sentence Sentence to build the correction lattice from.
	///
	////////////////////////////////////////////////////////////////////////////
	void buildLatticeFromSentence(std::vector< std::vector<Trio> > & probas, const std::vector<unsigned int> & sentence);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Perform the Viterbi algorithm for the specified row of the lattice.
	///
	/// \param probas Lattice the algorithm will be performed on.
	/// \param row Row the algorithm will be performed on.
	///
	////////////////////////////////////////////////////////////////////////////
	void computeViterbiForRow(std::vector< std::vector<Trio> > & probas, unsigned int row);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Neatly output the current lattice into `stdout`, human readable.
	///
	/// \param probas Lattice to print.
	///
	////////////////////////////////////////////////////////////////////////////
	void printLatticeForDebug(std::vector< std::vector<Trio> > & probas);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Correct a sentence.
	///
	/// Correction of words are taken from every instances of WordTranslator found
	/// as a member of `database`.
	///
	/// \param dest Where to put the corrected sentence.
	/// \param sentence Sentence to be corrected, tokenized.
	///
	////////////////////////////////////////////////////////////////////////////
	void correctSentence(std::vector<unsigned int> & dest, const std::vector<unsigned int> & sentence);

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// \param database Database, has to be initialized prior to any call 
	///	to correct()
	///
	////////////////////////////////////////////////////////////////////////////
	Viterbi(Database & database);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Correct a text file.
	///
	/// \param inputFilename Path to file containing the text to correct.
	///
	/// \return File containing the corrected text.
	///
	////////////////////////////////////////////////////////////////////////////
	File * correct(std::string inputFilename);
};

#endif

