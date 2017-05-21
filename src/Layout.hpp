/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef LAYOUT__H
#define LAYOUT__H

#include "File.hpp"
#include <vector>
#include <set>

/////////////////////////////////////////////////////////////////////////////
/// \brief Transfers the punctuation from one file to another.
///
/// Example :
///
/// if `file1` contains "[06:20:44] _CLIENT_ : Sesi est une frase... pleines de fotes !"
/// and `file2` contains "ceci est une phrase pleine de fautes"
///
/// \code
/// Layout layout(file1, file2);
/// File file3("path/to/file3.txt", "w");
/// layout.transferLayout(file3);
/// \endcode
/// Will produce a file containing
/// "[06:20:44] _CLIENT_ : ceci est une phrase... pleine de fautes !"
///
/// The purpose is to re-introduce the syntax from original un-corrected text to
/// the syntax-less corrected text.
////////////////////////////////////////////////////////////////////////////
class Layout
{
	private :

	File * baseText;
	File * correctedText;
	std::vector< std::set<unsigned int> > fusions;

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// \param baseText File containing text with syntax.
	/// \param correctedText File containing text without syntax.
	/// \param fusions One set per line, index of fused words.
	///
	////////////////////////////////////////////////////////////////////////////
	Layout(File * baseText, File * correctedText, std::vector< std::set<unsigned int> > & fusions);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Write the contents of correctedText with the syntax of baseText.
	///
	/// \param target File in which to write.
	///
	////////////////////////////////////////////////////////////////////////////
	void transferLayout(File * target);
};

#endif

