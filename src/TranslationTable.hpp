/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef TRANSLATIONTABLE__H
#define TRANSLATIONTABLE__H

#include <set>
#include "File.hpp"
#include "Lexicon.hpp"
#include "AtomicFloat.hpp"
#include "WordTranslator.hpp"

/////////////////////////////////////////////////////////////////////////////
/// \brief Proposes corrections of a word based on a model learned from 
/// manually corrected corpus.
///
/// \see WordTranslator, Viterbi
///
////////////////////////////////////////////////////////////////////////////
class TranslationTable : public WordTranslator
{
	private :

	static constexpr int nbIterations = 10;
	static float minimalProb;

	std::vector< std::vector<AtomicFloat> > table;

	unsigned int maxTokenCorrect;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Check if a token correspond to a correct word.
	///
	/// \param token Token to check.
	///
	/// \return true if `token` correspond to a correct word.
	///
	////////////////////////////////////////////////////////////////////////////
	bool isCorrect(unsigned int token);

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Construct empty TranslationTable
	///
	////////////////////////////////////////////////////////////////////////////
	TranslationTable();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Compute the TranslationTable from manually corrected corpus.
	///
	/// This can be very slow depending on the size of the corpus.
	/// However it should only be used once per corpus since it can be stored and
	/// loaded from a file.
	///
	/// \param correctLexicon Lexicon containing only correct words.
	/// \param incorrectLexicon Lexicon containing both correct and incorrect words.
	/// \param incorrect File containing the un-corrected version of the corpus.
	/// \param correct File containing the corrected version of the corpus.
	///
	////////////////////////////////////////////////////////////////////////////
	void create(const Lexicon & correctLexicon, const Lexicon & incorrectLexicon,
		File & incorrect, File & correct);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Save this TranslationTable into a file.
	///
	/// \param output File descriptor the TranslationTable will be written into.
	/// \see read()
	///
	////////////////////////////////////////////////////////////////////////////
	void print(FILE * output);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Print human readable representation of this TranslationTable.
	///
	/// Useful only for debug purpose.
	///
	/// \param output File descriptor the TranslationTable will be written into.
	/// \param correctLex Lexicon containing only correct words.
	/// \param incorrectLex Lexicon containing both correct and incorrect words.
	///
	////////////////////////////////////////////////////////////////////////////
	void printForDebug(FILE * output, Lexicon & correctLex,
		Lexicon & incorrectLex);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Initialize this TranslationTable from a file.
	///
	/// \param input File containing the TranslationTable.
	/// \see print()
	///
	////////////////////////////////////////////////////////////////////////////
	void read(File & input);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Proposes corrections for an incorrect word, using what has been
	/// learned from the corpora.
	///
	/// \param actual 
	/// \see WordTranslator::addTranslations()
	///
	////////////////////////////////////////////////////////////////////////////
	void addTranslations(WordTranslations & actual);
};

#endif
