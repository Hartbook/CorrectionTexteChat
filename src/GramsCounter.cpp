#include "GramsCounter.hpp"
#include <cmath>
#include "Lexicon.hpp"
#include "util.hpp"

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

GramsCounter::GramsCounter()
{
	nbMonograms = 0;
	nbBigrams = 0;
	nbTrigrams = 0;
	nbTokensTotal = 0;
}

void GramsCounter::addGram(unsigned int t1)
{
	Gram gram(t1);

	nbMonograms++;

	nbOcc[gram]++;

	nbTokensTotal++;
}

void GramsCounter::addGram(unsigned int t1, unsigned int t2)
{
	Gram gram(t1, t2);

	nbBigrams++;

	nbOcc[gram]++;
}

void GramsCounter::addGram(unsigned int t1, unsigned int t2, unsigned int t3)
{
	Gram gram(t1, t2, t3);

	nbTrigrams++;

	nbOcc[gram]++;
}

void GramsCounter::read(File & input)
{
	FILE * desc = input.getDescriptor();
	nbOcc.clear();
	unsigned int nbElems = 0;
	fread(&nbElems, sizeof nbElems, 1, desc);

	nbOcc.reserve(nbElems);

	Gram gram(0,0,0);

	for (unsigned int i = 0; i < nbElems; i++)
	{
		unsigned int nbTokens = 0;

		fread(&nbTokens, sizeof nbTokens, 1, desc);

		gram.tokens.clear();

		for (unsigned int j = 0; j < nbTokens; j++)
		{
			unsigned int token;
			fread(&token, sizeof token, 1, desc);
			gram.tokens.emplace_back(token);
		}

		int nb = 0;
		fread(&nb, sizeof nb, 1, desc);

		nbOcc[gram] = nb;
	}

	fread(&nbMonograms, sizeof nbMonograms, 1, desc);
	fread(&nbBigrams, sizeof nbBigrams, 1, desc);
	fread(&nbTrigrams, sizeof nbTrigrams, 1, desc);
	fread(&alpha, sizeof alpha, 1, desc);
	fread(&nbTokensTotal, sizeof nbTokensTotal, 1, desc);
}

void GramsCounter::print(FILE * output)
{
	unsigned int nbElems = nbOcc.size();
	fwrite(&nbElems, sizeof nbElems, 1, output);

	for (auto & it : nbOcc)
	{
		unsigned int nbTokens = it.first.tokens.size();
		fwrite(&nbTokens, sizeof nbTokens, 1, output);
		fwrite(&it.first.tokens[0], sizeof it.first.tokens[0], nbTokens, output);
		fwrite(&it.second, sizeof it.second, 1, output);
	}

	fwrite(&nbMonograms, sizeof nbMonograms, 1, output);
	fwrite(&nbBigrams, sizeof nbBigrams, 1, output);
	fwrite(&nbTrigrams, sizeof nbTrigrams, 1, output);
	fwrite(&alpha, sizeof alpha, 1, output);
	fwrite(&nbTokensTotal, sizeof nbTokensTotal, 1, output);
}

void GramsCounter::print(FILE * output, Lexicon & lexicon)
{
	for (auto it : nbOcc)
	{
		float logProb = 9999;
		if (it.first.tokens.size() == 1)
			logProb = getLogProb(it.first.tokens[0]);
		else if (it.first.tokens.size() == 2)
			logProb = getLogProb(it.first.tokens[0], it.first.tokens[1]);
		else if (it.first.tokens.size() == 3)
			logProb = getLogProb(it.first.tokens[0], it.first.tokens[1], it.first.tokens[2]);

		fprintf(output, "%07d %5f ", it.second, logProb);

		fprintf(output, "<");

		for (unsigned int i = 0; i < it.first.tokens.size(); i++)
		{
			const auto & word = lexicon.getString(it.first.tokens[i]);

			fprintf(output, "%s%s", word.c_str(), i == it.first.tokens.size()-1 ? ">\n" : ", ");
		}
	}
}

float GramsCounter::getProb(unsigned int t1) const
{
	if (nbOcc.count(Gram(t1)))
		return ((float)nbOcc.at(Gram(t1))) / nbMonograms;

	return alpha / (nbTokensTotal*(1+alpha));
}

float GramsCounter::getProb(unsigned int t1, unsigned int t2) const
{
	if (nbOcc.count(Gram(t1,t2)))
		return ((float)nbOcc.at(Gram(t1,t2))) / nbOcc.at(Gram(t1));

	return alpha / ((nbOcc.count(Gram(t1)) ? nbOcc.at(Gram(t1)) : 0) + nbTokensTotal*alpha);
}

float GramsCounter::getProb(unsigned int t1, unsigned int t2, unsigned int t3) const
{
	if (nbOcc.count(Gram(t1,t2,t3)))
		return ((float)nbOcc.at(Gram(t1,t2,t3))) / nbOcc.at(Gram(t1,t2));

	return alpha / ((nbOcc.count(Gram(t1,t2)) ? nbOcc.at(Gram(t1,t2)) : 0)+nbTokensTotal*alpha);
}

float GramsCounter::getLogProb(unsigned int t1) const
{
	return -log(getProb(t1));
}

float GramsCounter::getLogProb(unsigned int t1, unsigned int t2) const
{
	return -log(getProb(t1,t2));
}

float GramsCounter::getLogProb(unsigned int t1, unsigned int t2, unsigned int t3) const
{
	return -log(getProb(t1,t2,t3));
}

