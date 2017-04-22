#include "TranslationTable.hpp"
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>

void TranslationTable::create(File & incorrect, File & correct)
{
	std::vector<unsigned int> sentenceIncorrect;
	std::vector<unsigned int> sentenceCorrect;

	auto readSentence = [&](std::vector<unsigned int> & sentence,
		std::set<unsigned int> & tokens, File & file)
	{
		sentence.clear();

		while (!file.isFinished() && file.peek() != '\n')
		{
			unsigned int token;

			fscanf(file.getDescriptor(), "%u", &token);

			sentence.push_back(token);
			tokens.insert(token);

			file.getChar();
		}

		file.getChar();
	};

	std::unordered_map<Pair, int> nb;
	std::set<unsigned int> tokensCorrect;
	std::set<unsigned int> tokensIncorrect;

	float initValueForTable = 1.0 / 1000000;

	for (int i = 0; i < nbIterations; i++)
	{
		std::unordered_map<Pair, float> nb;
		std::unordered_map<unsigned int, float> total;

		while (!incorrect.isFinished() && !correct.isFinished())
		{
			readSentence(sentenceIncorrect, tokensIncorrect, incorrect);
			readSentence(sentenceCorrect, tokensCorrect, correct);

			std::unordered_map<unsigned int, float> s_total;

			for (auto tokenIncorrect : sentenceIncorrect)
			{
				s_total[tokenIncorrect] = 0;
	
				for (auto tokenCorrect : sentenceCorrect)
				{
					if (i == 0 && table[Pair(tokenIncorrect, tokenCorrect)] == 0)
						table[Pair(tokenIncorrect, tokenCorrect)] = initValueForTable;
					s_total[tokenIncorrect] += table[Pair(tokenIncorrect, tokenCorrect)];
				}
			}

			for (auto tokenIncorrect : sentenceIncorrect)
				for (auto tokenCorrect : sentenceCorrect)
				{
					if (i == 0 && table[Pair(tokenIncorrect, tokenCorrect)] == 0)
						table[Pair(tokenIncorrect, tokenCorrect)] = initValueForTable;

					auto increment = table[Pair(tokenIncorrect, tokenCorrect)];

					nb[Pair(tokenIncorrect, tokenCorrect)] += increment/s_total[tokenIncorrect];
					total[tokenCorrect] +=
						table[Pair(tokenIncorrect, tokenCorrect)] / s_total[tokenIncorrect];
				}
		}

		for (auto tokenIncorrect : tokensIncorrect)
			for (auto tokenCorrect : tokensCorrect)
			{
				if (i == 0 && table[Pair(tokenIncorrect, tokenCorrect)] == 0)
					table[Pair(tokenIncorrect, tokenCorrect)] = initValueForTable;

				table[Pair(tokenIncorrect, tokenCorrect)] =
					nb[Pair(tokenIncorrect, tokenCorrect)] / total[tokenCorrect];
			}

		incorrect.rewind();
		correct.rewind();

		printf("Iteration %d : OK !\n", i);
	}

	for (auto & it : table)
		if (it.second >= minimalProb)
		 	it.second = -log(it.second);
		else
			it.second = -1.0;
}

void TranslationTable::print(FILE * output)
{
	for (auto & it : table)
		if (it.second >= 0)
			fprintf(output, "%u %u %f\n", it.first.first, it.first.second, it.second);
}

void TranslationTable::printForDebug(FILE * output, Lexicon & correctLex,
	Lexicon & incorrectLex)
{
	using Elem = std::pair<std::pair<unsigned int, unsigned int>, float>;

	std::vector<Elem> sortedPairs;

	for (auto & it : table)
		if (it.second >= 0)
			sortedPairs.push_back(it);

	std::sort(sortedPairs.begin(), sortedPairs.end(),
	[](Elem & a, Elem & b)
	{
		return a.second < b.second;
	});

	for (auto & it : sortedPairs)
		fprintf(output, "%09f\t<%s> -> <%s>\n", it.second,
			incorrectLex.getString(it.first.first).c_str(),
			correctLex.getString(it.first.second).c_str());
}

void TranslationTable::read(File & input)
{
	table.clear();
	unsigned int token1, token2;
	float proba;

	while (fscanf(input.getDescriptor(), "%u %u %f", &token1, &token2, &proba) == 3)
		table[Pair(token1, token2)] = proba;
}

