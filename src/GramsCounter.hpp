#ifndef GRAMSCOUNTER__H
#define GRAMSCOUNTER__H

#include <vector>

class GramsCounter
{
	private :

	class Gram
	{
		protected :

		unsigned int nbOcc;
		std::vector<unsigned int> tokens;
		Gram();

		public :

		void countUp();
		bool match(std::vector<unsigned int> & model);
		unsigned int getNbOcc();
	};

	std::vector<Gram> monograms;
	std::vector<Gram> bigrams;
	std::vector<Gram> trigrams;

	public :

	class Monogram : Gram
	{
		Monogram(unsigned int token);
	};

	class Bigram : Gram
	{
		Bigram(unsigned int t1, unsigned int t2);
	};

	class Trigram : Gram
	{
		Trigram(unsigned int t1, unsigned int t2, unsigned int t3);
	};
};

#endif
