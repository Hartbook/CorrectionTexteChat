#ifndef GRAMSCOUNTER__H
#define GRAMSCOUNTER__H

#include <unordered_map>
#include <vector>

class Lexicon;
class Gram;
class GramsCounter;

namespace std {struct hash<Gram>;}

class Gram
{
	private :

	std::vector<unsigned int> tokens;

	Gram(unsigned int t1);
	Gram(unsigned int t1, unsigned int t2);
	Gram(unsigned int t1, unsigned int t2, unsigned int t3);

	public :

	bool operator==(const Gram & other) const;
	
	friend class std::hash<Gram>;
	friend class GramsCounter;
};

namespace std
{
	template <>
	struct hash<Gram>
	{
		size_t operator()(const Gram & g) const
		{
    		static hash<unsigned int> hasher;

			size_t seed = 0;

			// From Boost
			for (auto it : g.tokens)
	    		seed ^= hasher(it) + 0x9e3779b9 + (seed<<6) + (seed>>2);

			return seed;
		}
	};
}

class GramsCounter
{
	private :

	std::unordered_map<Gram, unsigned int> nbOcc;

	public :

	void addGram(unsigned int t1);
	void addGram(unsigned int t1, unsigned int t2);
	void addGram(unsigned int t1, unsigned int t2, unsigned int t3);

	void print(FILE * output);
	void print(FILE * output, Lexicon & lexicon);
};

#endif
