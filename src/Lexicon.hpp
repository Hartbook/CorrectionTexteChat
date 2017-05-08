/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef LEXICON__H
#define LEXICON__H

#include <string>
#include <unordered_map>
#include "File.hpp"

/////////////////////////////////////////////////////////////////////////////
/// \brief Contains known words and their corresponding token.
///
////////////////////////////////////////////////////////////////////////////
class Lexicon
{
	private :

	std::unordered_map<std::string, unsigned int> tokens;
	std::unordered_map<unsigned int, const std::string *> strings;
	unsigned int nextToken;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Make the input string lower case.
	///
	/// Used before insertion and look-up.
	///
	/// \param s String to transform.
	///
	/// \return reference to s.
	///
	////////////////////////////////////////////////////////////////////////////
	std::string & normalize(std::string & s);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Insert a word into the Lexicon associated with a specific token.
	///
	/// This is used by copy().
	///
	/// \param word Word to insert.
	/// \param token Token corresponding to the word.
	///
	////////////////////////////////////////////////////////////////////////////
	void addWord(std::string word, unsigned int token);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Insert special entries to the Lexicon.
	///
	/// Used by the constructor.
	///
	/// \param specials Special entries like Lexicon::unknown
	///
	////////////////////////////////////////////////////////////////////////////
	void initMaps(std::unordered_map<std::string, unsigned int> & specials);

	public :

	static constexpr unsigned int unknown = 0;
	static constexpr char unknownStr[] = "UNKNOWN";

	static constexpr unsigned int mail = 1;
	static constexpr char mailStr[] = "MAIL";

	static constexpr unsigned int number = 2;
	static constexpr char numberStr[] = "NUMBER";

	static constexpr unsigned int properNoun = 3;
	static constexpr char properNounStr[] = "PROPERNOUN";

	static constexpr unsigned int date = 4;
	static constexpr char dateStr[] = "DATE";

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	////////////////////////////////////////////////////////////////////////////
	Lexicon();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Tokenize a word.
	///
	/// \param s Word to tokenize.
	///
	/// \return Token corresponding to `s`, Lexicon::unknown if `s` is not in the
	/// Lexicon.
	///
	////////////////////////////////////////////////////////////////////////////
	unsigned int getToken(std::string & s);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Get every word and token inside the Lexicon.
	///
	/// \return const ref to container.
	///
	////////////////////////////////////////////////////////////////////////////
	const std::unordered_map<std::string, unsigned int> & getTokens();

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Get the word corresponding to a token.
	///
	/// \param token Token of the word.
	///
	/// \return The word which token is `token`, Lexicon::unknownStr if `token`
	/// is not in the Lexicon.
	///
	////////////////////////////////////////////////////////////////////////////
	const std::string & getString(unsigned int token);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Add a word to the Lexicon.
	///
	/// The corresponding token will be chosen sequentialy so that there is no gap.
	///
	/// \param word Word to add.
	///
	/// \return The token corresponding to `word`
	///
	////////////////////////////////////////////////////////////////////////////
	unsigned int addWord(const std::string & word);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Save the Lexicon into a File.
	///
	/// \param output File descriptor the Lexicon will be saved into.
	/// \see read()
	////////////////////////////////////////////////////////////////////////////
	void print(FILE * output);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Initialize this Lexicon from a file.
	///
	/// \param input File to read the Lexicon from, as writen by print()
	/// \see print()
	////////////////////////////////////////////////////////////////////////////
	void read(File & input);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Initialize this Lexicon so that it contains every word present in
	/// other.
	///
	/// \param other The Lexicon to copy from.
	///
	////////////////////////////////////////////////////////////////////////////
	void copy(const Lexicon & other);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Get the biggest token present in this instance.
	///
	/// if token > lexicon.getMaxToken() then token do not belong to lexicon.
	///
	/// \return The biggest token present in this instance.
	///
	////////////////////////////////////////////////////////////////////////////
	unsigned int getMaxToken() const;
};

#endif
