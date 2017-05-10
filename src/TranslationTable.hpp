/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef TRANSLATIONTABLE__H
#define TRANSLATIONTABLE__H

#include <vector>
#include <set>
#include <unordered_map>
#include "File.hpp"
#include "Lexicon.hpp"
#include "AtomicFloat.hpp"
#include "WordTranslator.hpp"

namespace std
{
	template <>
	struct hash< pair<unsigned int, unsigned int> >
	{
		size_t operator()(const pair<unsigned int, unsigned int> & p) const
		{
    		static hash<unsigned int> hasher;

			size_t seed = 0;

			// From Boost
	    	seed ^= hasher(p.first) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	    	seed ^= hasher(p.second) + 0x9e3779b9 + (seed<<6) + (seed>>2);

			return seed;
		}
	};
}

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

	std::unordered_map<std::pair<unsigned int, unsigned int>, AtomicFloat> table;

	std::unordered_map< unsigned int,std::vector< std::pair<unsigned int,float> > > translations;

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

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Transfer contents of `table` into `translations` for faster lookup.
	///
	/// To be called before the first call to addTranslations().
	///
	////////////////////////////////////////////////////////////////////////////
	void arrangeTranslations();

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
