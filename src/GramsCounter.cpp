#include "GramsCounter.hpp"

Gram::Gram(unsigned int t1)
{
	tokens.push_back(t1);
}

Gram::Gram(unsigned int t1, unsigned int t2)
{
	tokens.push_back(t1);
	tokens.push_back(t2);
}

Gram::Gram(unsigned int t1, unsigned int t2, unsigned int t3)
{
	tokens.push_back(t1);
	tokens.push_back(t2);
	tokens.push_back(t3);
}

bool Gram::operator==(const Gram & other) const
{
	return other.tokens == tokens;
}

void GramsCounter::addGram(unsigned int t1)
{
	nbOcc[Gram(t1)]++;
}

void GramsCounter::addGram(unsigned int t1, unsigned int t2)
{
	nbOcc[Gram(t1, t2)]++;
}

void GramsCounter::addGram(unsigned int t1, unsigned int t2, unsigned int t3)
{
	nbOcc[Gram(t1, t2, t3)]++;
}

void GramsCounter::print(FILE * output)
{
	for (auto it : nbOcc)
	{
		fprintf(output, "<");

		for (unsigned int i = 0; i < it.first.tokens.size(); i++)
			fprintf(output, "%03d%s", it.first.tokens[i], i == it.first.tokens.size()-1 ? ">" : ", ");

		for (unsigned int i = 0; i < 30 - 5*it.first.tokens.size(); i++)
			fprintf(output, " ");

		fprintf(output, "%d\n", it.second);
	}
}

