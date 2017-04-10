#include "GramsCounter.hpp"

#include <cstdio>

GramsCounter::Gram::Gram()
{
	nbOcc = 0;
	printf("OK\n");
}

void GramsCounter::Gram::countUp()
{
	nbOcc++;
}

bool GramsCounter::Gram::match(std::vector<unsigned int> & model)
{
	return tokens == model;
}

unsigned int GramsCounter::Gram::getNbOcc()
{
	return nbOcc;
}

GramsCounter::Monogram::Monogram(unsigned int token)
{
	tokens.push_back(token);
}

GramsCounter::Bigram::Bigram(unsigned int t1, unsigned int t2)
{
	tokens.push_back(t1);
	tokens.push_back(t2);
}

GramsCounter::Trigram::Trigram(unsigned int t1, unsigned int t2, unsigned int t3)
{
	tokens.push_back(t1);
	tokens.push_back(t2);
	tokens.push_back(t3);
}

