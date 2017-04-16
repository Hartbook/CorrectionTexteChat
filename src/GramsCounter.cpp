#include "GramsCounter.hpp"
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

		unsigned int padding = 50;

		for (unsigned int i = 0; i < it.first.tokens.size(); i++)
		{
			unsigned int token = it.first.tokens[i];

			fprintf(output, "%06d%s", token, i == it.first.tokens.size()-1 ? ">" : ", ");

			padding -= 2;
			padding -= 6;
		}

		for (unsigned int i = 0; i < padding; i++)
			fprintf(output, " ");

		fprintf(output, "%d\n", it.second);
	}
}

void GramsCounter::print(FILE * output, Lexicon & lexicon)
{
	static unsigned int maxPadding = 50;

	for (auto it : nbOcc)
	{
		fprintf(output, "<");

		unsigned int padding = maxPadding;

		for (unsigned int i = 0; i < it.first.tokens.size(); i++)
		{
			const auto & word = lexicon.getString(it.first.tokens[i]);

			fprintf(output, "%s%s", word.c_str(), i == it.first.tokens.size()-1 ? ">" : ", ");

			padding -= 2;
			padding -= lengthPrinted(word);
		}

		if (padding > maxPadding)
			padding = 1;

		for (unsigned int i = 0; i < padding; i++)
			fprintf(output, " ");

		fprintf(output, "%d\n", it.second);
	}
}

