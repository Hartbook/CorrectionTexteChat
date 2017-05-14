/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef DATABASE__H
#define DATABASE__H

#include <memory>
#include <string>
#include <vector>
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

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Construct lexicons from files.
	///
	/// \param filenames List of paths to file that contains lexicons.
	///
	////////////////////////////////////////////////////////////////////////////
	void buildLexiconFromCorpora(const std::vector<std::string> & filenames);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Build the language model from files.
	///
	/// \param filenames List of paths to file that contains corpora to learn from.
	///
	////////////////////////////////////////////////////////////////////////////
	void buildGramsCounterFromCorpora(const std::vector<std::string> & filenames);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Build the TranslationTable from files.
	///
	/// \param filenames List of pairs of paths to files consisting of un-corrected and
	/// corrected versions of the same text.
	///
	////////////////////////////////////////////////////////////////////////////
	void buildTranslationTableFromCorpora(const std::vector< std::pair<std::string, std::string> > & filenames);

	friend Viterbi;

	public :

	////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// When constructed the Database represents nothing and must not be used as it is.
	/// You have to call buildFromCorpus or readFromFiles in order to initialize it.
	///
	////////////////////////////////////////////////////////////////////////////
	Database();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Build the Database, long process.
	///
	/// Construct Lexicon, GramsCounter, TranslationTable using the files passed
	/// as parameters.
	/// Creates a number of files, that can later be used for readFromFiles().
	///
	/// \param lexicons List of paths to files that contain a lexicon.
	/// \param corpora List of paths to files that contain text to learn ngrams from.
	/// \param pairs List of pairs of paths to files consisting of un-corrected and
	/// corrected versions of the same text.
	///
	////////////////////////////////////////////////////////////////////////////
	void buildFromCorpus(const std::vector<std::string> & lexicons,
		const std::vector<std::string> & corpora,
		const std::vector< std::pair<std::string, std::string> > & pairs);

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
};

#endif
