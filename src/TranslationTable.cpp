#include "TranslationTable.hpp"
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>
#include <atomic>
#include "Executor.hpp"

void TranslationTable::create(const Lexicon & correctLexicon,
	const Lexicon & incorrectLexicon, File & incorrect, File & correct)
{
	auto isCorrect = [&](unsigned int token)
	{
		return token <= correctLexicon.getMaxToken();
	};

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

	std::set<unsigned int> tokensCorrect;
	std::set<unsigned int> tokensIncorrect;

	std::vector< std::vector<unsigned int> > correctSentences;
	std::vector< std::vector<unsigned int> > incorrectSentences;

	while (!incorrect.isFinished() && !correct.isFinished())
	{
		correctSentences.emplace_back();
		incorrectSentences.emplace_back();

		readSentence(incorrectSentences.back(), tokensIncorrect, incorrect);
		readSentence(correctSentences.back(), tokensCorrect, correct);
	}

	unsigned int nbSentences = correctSentences.size();
	if (incorrectSentences.size() > nbSentences)
		nbSentences = incorrectSentences.size();

	float initValueForTable = 1.0 / (correctLexicon.getMaxToken()+1);

	for (auto tokenIncorrect : tokensIncorrect)
	{
		if (isCorrect(tokenIncorrect))
			continue;

		for (auto tokenCorrect : tokensCorrect)
			table[Pair(tokenIncorrect, tokenCorrect)] = initValueForTable;
	}

	Executor<void,void> executor;

	std::unordered_map<unsigned int, AtomicFloat> total;
	std::unordered_map<Pair, AtomicFloat> nb;

	for (auto tokenIncorrect : tokensIncorrect)
	{
		if (isCorrect(tokenIncorrect))
			continue;

		for (auto tokenCorrect : tokensCorrect)
			nb[Pair(tokenIncorrect, tokenCorrect)] = 0;
	}

	for (auto tokenCorrect : tokensCorrect)
		total[tokenCorrect] = 0;

	for (int i = 0; i < nbIterations; i++)
	{
		executor.clear();
	
		for (auto & it : nb)
			it.second = 0.0f;

		for (auto & it : total)
			it.second = 0.0f;

		for (unsigned int sentenceIndex = 0; sentenceIndex < nbSentences; sentenceIndex++)
			executor.addTask([this, sentenceIndex, &incorrectSentences, &correctSentences,
								&incorrectLexicon, &isCorrect, &total, &nb]()
			{
				auto & sentenceIncorrect = incorrectSentences[sentenceIndex];
				auto & sentenceCorrect = correctSentences[sentenceIndex];

				std::vector<float> s_total(incorrectLexicon.getMaxToken()+1, 0.0f);

				for (auto tokenIncorrect : sentenceIncorrect)
				{
					if (isCorrect(tokenIncorrect))
						continue;

					for (auto tokenCorrect : sentenceCorrect)
						s_total[tokenIncorrect] += table[Pair(tokenIncorrect, tokenCorrect)];
				}

				for (auto tokenIncorrect : sentenceIncorrect)
				{
					if (isCorrect(tokenIncorrect))
						continue;

					for (auto tokenCorrect : sentenceCorrect)
					{
						Pair p(tokenIncorrect, tokenCorrect);

						nb[p] += table[p] / s_total[tokenIncorrect];

						total[tokenCorrect] += table[p] / s_total[tokenIncorrect];
					}
				}
			});

		executor.run();

		executor.clear();

		for (auto tokenIncorrect : tokensIncorrect)
		{
			if (isCorrect(tokenIncorrect))
				continue;
			
			executor.addTask([this, &nb, &total, &tokensCorrect, tokenIncorrect]()
			{
				for (auto tokenCorrect : tokensCorrect)
				{
					Pair p(tokenIncorrect, tokenCorrect);
					table[p] = nb[p] / total[tokenCorrect];
				}
			});
		}

		executor.run();

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
			fprintf(output, "%u %u %f\n", it.first.first, it.first.second, it.second.load());
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
		if (a.first.first == b.first.first)
			return a.second < b.second;

		return a.first.first < b.first.first;
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

