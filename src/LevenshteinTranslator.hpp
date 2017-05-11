/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef LEVENSHTEINTRANSLATOR__H
#define LEVENSHTEINTRANSLATOR__H

#include "Lexicon.hpp"
#include "WordTranslator.hpp"

/////////////////////////////////////////////////////////////////////////////
/// \brief Proposes corrections of a word based on Levenshtein distances from
/// correct words.
///
/// \see WordTranslator, Viterbi
////////////////////////////////////////////////////////////////////////////
class LevenshteinTranslator : public WordTranslator
{
	private :

	Lexicon & correctLexicon;
	Lexicon & incorrectLexicon;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Return the cost of substituing i-th element of s1 by the j-th 
	/// element of s2.
	///
	/// If it returns 1 if s1[i] != s2[j] and 0 otherwise, getProximity() will
	/// return the edit distance. However as return value is `float` it can be tuned
	/// to penalize unlikely substitutions.
	///
	/// \param s1 String containing the original letter.
	/// \param s2 String containing the transformed letter.
	/// \param i Index of the original letter.
	/// \param j Index of the transformed letter.
	///
	/// \return 0 if s1[i] != s2[j], the more it's high the less likely the
	/// substitution is.
	///
	////////////////////////////////////////////////////////////////////////////
	float getSubstitutionCost(const std::string & s1, const std::string & s2,
							  unsigned int i, unsigned int j);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Return the cost of adding j-th element of s2 at i position in s1.
	///
	/// \param s1 String containing the original letter.
	/// \param s2 String containing the transformed letter.
	/// \param i Index of the insertion.
	/// \param j Index of the letter to insert.
	/// \param addHasOccured
	///
	/// \return the more it's high the less likely the insertion is.
	///
	////////////////////////////////////////////////////////////////////////////
	float getAddCost(const std::string & s1, const std::string & s2,
							  unsigned int i, unsigned int j, bool & addHasOccured);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Return the cost of deleting i-th element of s1.
	///
	/// \param s1 String containing the original letter.
	/// \param s2 String containing the transformed letter.
	/// \param i Index of the deletion.
	/// \param delHasOccured
	///
	/// \return the more it's high the less likely the deletion is.
	///
	////////////////////////////////////////////////////////////////////////////
	float getDelCost(const std::string & s1, const std::string & s2,
							  unsigned int i, unsigned int j, bool & delHasOccured);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Get the distance between two strings.
	///
	/// \param s1 First string to check.
	/// \param s2 Second string to check.
	///
	/// \return 0 if s1 == s2, the more it's high the less likely it is that
	/// s2 is the correction of s1.
	///
	////////////////////////////////////////////////////////////////////////////
	float getProximity(const std::string & s1, const std::string & s2);

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// \param correctLexicon A Lexicon containing every correct word.
	/// \param incorrectLexicon A Lexicon containing corrects and incorrects words.
	///
	////////////////////////////////////////////////////////////////////////////
	LevenshteinTranslator(Lexicon & correctLexicon, Lexicon & incorrectLexicon);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Proposes corrections for an incorrect word, using getProximity()
	/// with correct words from `correctLexicon`.
	///
	/// \param actual
	/// \see WordTranslator::addTranslations()
	///
	////////////////////////////////////////////////////////////////////////////
	void addTranslations(WordTranslations & actual);
};

#endif
