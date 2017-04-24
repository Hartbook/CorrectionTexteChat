#ifndef TRANSLATIONTABLE__H
#define TRANSLATIONTABLE__H

#include <unordered_map>
#include "File.hpp"
#include "Lexicon.hpp"
#include "AtomicFloat.hpp"

class TranslationTable;
namespace std {struct hash< std::pair<unsigned int, unsigned int> >;}

namespace std
{
	template <>
	struct hash< std::pair<unsigned int, unsigned int> >
	{
		size_t operator()(const std::pair<unsigned int, unsigned int> & p) const
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

class TranslationTable
{
	private :

	static constexpr int nbIterations = 5;
	static constexpr float minimalProb = 0.001;

	using Pair = std::pair<unsigned int, unsigned int>;

	std::unordered_map<Pair, AtomicFloat> table;

	public :

	void create(const Lexicon & correctLexicon, const Lexicon & incorrectLexicon,
		File & incorrect, File & correct);
	void print(FILE * output);
	void printForDebug(FILE * output, Lexicon & correctLex,
		Lexicon & incorrectLex);
	void read(File & input);
};

#endif
