/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef DATABASE__H
#define DATABASE__H

#include <string>
#include "Lexicon.hpp"
#include "GramsCounter.hpp"
#include "TranslationTable.hpp"
#include "LevenshteinTranslator.hpp"

class Viterbi;

////////////////////////////////////////////////////////////////////////////
/// \brief Contains necessary data to perform a correction.
///
/// Creation of the Database is a long process, that is why you only need to
/// build it once.
/// During the build process several files representing the Database will be written.
/// Database can then be directly loaded from those files.
///
////////////////////////////////////////////////////////////////////////////
class Database
{
	private :

	Lexicon correctLexicon;
	Lexicon incorrectLexicon;
	GramsCounter gramsCounter;
	TranslationTable translationTable;
	LevenshteinTranslator levenshteinTranslator;

	////////////////////////////////////////////////////////////////////////////
	/// \brief WILL BE CHANGED SOON
	///
	/// \param lexicon
	/// \param corpus
	/// \param countGrams
	///
	////////////////////////////////////////////////////////////////////////////
	void buildLexiconFromCorpus(Lexicon & lexicon, File & corpus, bool countGrams);

	public :

	////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// When constructed the Database represents nothing and must not be used as it is.
	/// You have to call buildFromCorpus or readFromFiles in order to initialize it.
	///
	////////////////////////////////////////////////////////////////////////////
	Database();

	////////////////////////////////////////////////////////////////////////////
	/// \brief Build the Database, long process.
	///
	/// Construct Lexicon, GramsCounter, TranslationTable using the files passed
	/// as parameters.
	/// Creates a number of files, that can later be used for readFromFiles.
	///
	/// \param correctName Path to the file containing the corrected corpus.
	/// \param incorrectName Path to the file containing the un-corrected corpus.
	///
	/// \see readFromFiles
	///
	////////////////////////////////////////////////////////////////////////////
	void buildFromCorpus(std::string correctName, std::string incorrectName);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Read the Database from files created by buildFromCorpus
	///
	/// \param incorrectFilename Path to the file containing Lexicon incorrectLexicon.
	/// \param correctFilename Path to the file containing Lexicon correctLexicon.
	/// \param gramsFilename Path to the file containing the GramsCounter.
	/// \param tableFilename Path to the file containing the TranslationTable.
	///
	/// \see buildFromCorpus
	///
	////////////////////////////////////////////////////////////////////////////
	void readFromFiles(std::string incorrectFilename, std::string correctFilename,
		std::string gramsFilename, std::string tableFilename);

	friend Viterbi;
};

#endif
