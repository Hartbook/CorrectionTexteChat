/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef WORDTRANSLATOR__H
#define WORDTRANSLATOR__H

#include <vector>
#include <unordered_map>
#include <memory>

/////////////////////////////////////////////////////////////////////////////
/// \brief Represents possible corrections for a given word.
///
/// Each possible translation is associated with a
///	 -logprob (the lesser the likeliest).
///
/// \see WordTranslator
///
////////////////////////////////////////////////////////////////////////////
class WordTranslations
{
	public :

	using Pair = std::pair<unsigned int, float>;

	unsigned int token;
	std::vector< std::unique_ptr<Pair> > translations;
	std::unordered_map<unsigned int, Pair *> positions;

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Kind of combine logprob so that if two possible corrections are
	/// identical they only appear one time with a stronger probability.
	///
	/// \param p1 First -log probability.
	/// \param p2 Second -log probability.
	///
	/// \return -log probability at least better or equal than the best of the two 
	/// parameters.
	///
	////////////////////////////////////////////////////////////////////////////
	float combineProbas(float p1, float p2);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Set the token of the word this instance will 
	///	hold possible correcitons of.
	///
	/// \param token
	///
	////////////////////////////////////////////////////////////////////////////
	void initForToken(unsigned int token);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Add a probable correction, associated with its probability.
	///
	/// If the token given as parameter is already a part of possible corrections,
	/// it will not be added twice but instead its probability will 
	///	be strengthened using combineProbas().
	///
	/// \param token The probable correction.
	/// \param proba Its probability.
	///
	////////////////////////////////////////////////////////////////////////////
	void addTranslation(unsigned int token, float proba);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Check if this instance doesn't contain any probable correction.
	///
	/// \return True if this instance doesn't contain any probable correction.
	///
	////////////////////////////////////////////////////////////////////////////
	bool empty();
};

/////////////////////////////////////////////////////////////////////////////
/// \brief Interface. Represent an object that is able to propose 
///	probable corrections of a given word. 
///
////////////////////////////////////////////////////////////////////////////
class WordTranslator
{
	public :

	unsigned int order;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Proposes corrections for an incorrect word and add 
	///	them to `actual`.
	///
	/// \param actual Contains possible corrections added by 
	///	previous WordTranslator.
	///
	////////////////////////////////////////////////////////////////////////////
	virtual void addTranslations(WordTranslations & actual) = 0;
};

#endif

