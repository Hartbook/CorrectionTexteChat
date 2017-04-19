#include "TranslationTable.hpp"
#include <vector>
#include <set>
#include <cmath>

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

	float initValueForTable = 1.0 / 1;

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
					if (table[Pair(tokenIncorrect, tokenCorrect)] == 0)
						table[Pair(tokenIncorrect, tokenCorrect)] = initValueForTable;
					s_total[tokenIncorrect] += table[Pair(tokenIncorrect, tokenCorrect)];
				}
			}

			for (auto tokenIncorrect : sentenceIncorrect)
				for (auto tokenCorrect : sentenceCorrect)
				{
					auto increment = table[Pair(tokenIncorrect, tokenCorrect)];

					nb[Pair(tokenIncorrect, tokenCorrect)] += increment/s_total[tokenIncorrect];
					total[tokenCorrect] +=
						table[Pair(tokenIncorrect, tokenCorrect)] / s_total[tokenIncorrect];
				}
		}

		for (auto tokenIncorrect : sentenceIncorrect)
			for (auto tokenCorrect : sentenceCorrect)
				table[Pair(tokenIncorrect, tokenCorrect)] =
					nb[Pair(tokenIncorrect, tokenCorrect)] / total[tokenCorrect];
	}

	for (auto & it : table)
		if (it.second >= minimalProb)
		 	it.second = -log(it.second);
}

void TranslationTable::print(FILE * output)
{
	for (auto & it : table)
		if (it.second >= minimalProb)
			fprintf(output, "%u %u %f\n", it.first.first, it.first.second, it.second);
}

void TranslationTable::printMostProbableTranslation(FILE * output, Lexicon & correctLex,
	Lexicon & incorrectLex)
{
	Pair best;
	float bestProb = 100000;

	for (auto & it : table)
		if (it.second < bestProb)
		{
			best = it.first;
			bestProb = it.second;
		}

	printf("Best : %s -> %s\n", incorrectLex.getString(best.first).c_str(), correctLex.getString(best.second).c_str());
}

