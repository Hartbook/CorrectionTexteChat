///////////////////////////////////////////////////////////////////////////////
/// @mainpage
///
/// ## Abstract
/// The goal of this program is to correct spelling in texts.
/// It is necessary to train the program beforeand, using large amount of 
/// data such as lexicons, corpus to learn a language model from and 
/// manually corrected examples.
///
/// Example :
///
/// [00:16:34] _CLIENT_: Bjr, monsieu j'ai oune probeme avec ma livebox : lecran tv est noi.
///
/// is corrected into
///
/// [00:16:34] _CLIENT_: Bonjour, monsieur j'ai un problème avec ma livebox : écran tv est noir.
///
/// ## Build
/// make to build the program (main). Requires g++ version with support of C++11.
/// make doc to build the Doxygen documentation (docs/html). Requires Doxygen.
/// 
/// ## Usage
/// ### Training
///
///	\code
/// ./main --buildDatabase -lexicons <List of lexicons> 
/// 	-corpora <Corpora to learn from> -pairs <Pairs of incorrect-correct texts>
/// \endcode
/// Example :
/// \code
/// ./main --buildDatabase -lexicons data/corpus/corrige/lexicon1 data/corpus/corrige/lexicon2
/// 	-corpora data/corpus/brut/germinal data/corpus/brut/tintin
/// 	-pairs data/corpus/brut/texte1Faux data/corpus/corrige/texte1Juste
/// 	data/corpus/brut/texte2Faux data/corpus/corrige/texte2Juste
/// \endcode
///
/// This will create files in data/lexicon, data/gramsCount, data/translationTable
///
/// ### Correcting
///
///	\code
///	--correct incorrectLexicon correctLexicon gramsCount translationTable input
/// \endcode
///
/// Where incorrectLexicon, correctLexicon, gramsCount and translationTable are
/// the files created by --buildDatabase ; and input is the file to correct.
///
/// This will create a file containing the correction of input in data/output
///
///////////////////////////////////////////////////////////////////////////////
