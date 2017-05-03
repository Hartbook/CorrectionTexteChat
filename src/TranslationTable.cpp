#include "TranslationTable.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <atomic>
#include "Executor.hpp"

float TranslationTable::minimalProb = 0.1;

TranslationTable::TranslationTable()
{
	order = 0;
}

void TranslationTable::create(const Lexicon & correctLexicon,
	const Lexicon & incorrectLexicon, File & incorrect, File & correct)
{
	maxTokenCorrect = correctLexicon.getMaxToken();

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

	auto cleanSentences = [&](std::vector<unsigned int> & correct,
		std::vector<unsigned int> & incorrect)
	{
		for (unsigned int i = 0; i < incorrect.size(); i++)
		{
			if (!isCorrect(incorrect[i]))
				continue;

			for (unsigned int j = 0; j < correct.size(); j++)
			{
				if (incorrect[i] == correct[j])
				{
					incorrect[i] = incorrect.back();
					correct[i] = correct.back();

					incorrect.pop_back();
					correct.pop_back();

					i--;
					break;
				}
			}
		}
	};

	std::vector< std::vector<unsigned int> > correctSentences;
	std::vector< std::vector<unsigned int> > incorrectSentences;

	std::set<unsigned int> tokensCorrect;
	std::set<unsigned int> tokensIncorrect;

	while (!incorrect.isFinished() && !correct.isFinished())
	{
		correctSentences.emplace_back();
		incorrectSentences.emplace_back();

		readSentence(incorrectSentences.back(), tokensIncorrect, incorrect);
		readSentence(correctSentences.back(), tokensCorrect, correct);

		cleanSentences(correctSentences.back(), incorrectSentences.back());
	}

	unsigned int nbSentences = std::max(correctSentences.size(), incorrectSentences.size());

	float initValueForTable = 1.0 / (incorrectLexicon.getMaxToken()+1);

	std::vector<AtomicFloat> total(correctLexicon.getMaxToken()+1);
	std::vector< std::vector<AtomicFloat> > nb(incorrectLexicon.getMaxToken()+1,
		std::vector<AtomicFloat>(correctLexicon.getMaxToken()+1));

	table = nb;

	for (auto & it : table)
		for (auto & it2 : it)
			it2 = initValueForTable;

	Executor<void,void> executor;

	for (int i = 0; i < nbIterations; i++)
	{
		executor.clear();
	
		for (auto & it : nb)
			for (auto & it2 : it)
				it2 = 0.0f;

		for (auto & it : total)
			it = 0.0f;

		for (unsigned int sentenceIndex = 0; sentenceIndex < nbSentences; sentenceIndex++)
			executor.addTask([this, sentenceIndex, &incorrectSentences, &correctSentences,
								&incorrectLexicon, &total, &nb]()
			{
				auto & sentenceIncorrect = incorrectSentences[sentenceIndex];
				auto & sentenceCorrect = correctSentences[sentenceIndex];

				std::vector<float> s_total(incorrectLexicon.getMaxToken()+1, 0.0f);

				for (auto tokenIncorrect : sentenceIncorrect)
					if (!isCorrect(tokenIncorrect))
						for (auto tokenCorrect : sentenceCorrect)
							s_total[tokenIncorrect] += table[tokenIncorrect][tokenCorrect];

				for (auto tokenIncorrect : sentenceIncorrect)
					if (!isCorrect(tokenIncorrect))
						for (auto tokenCorrect : sentenceCorrect)
						{
							nb[tokenIncorrect][tokenCorrect] +=
								table[tokenIncorrect][tokenCorrect] / s_total[tokenIncorrect];
							total[tokenCorrect] += table[tokenIncorrect][tokenCorrect] /
								s_total[tokenIncorrect];
						}
			});

		executor.run();

		executor.clear();

		for (auto tokenIncorrect : tokensIncorrect)
			if (!isCorrect(tokenIncorrect))
				executor.addTask([this, &nb, &total, &tokensCorrect, tokenIncorrect]()
				{
					for (auto tokenCorrect : tokensCorrect)
						table[tokenIncorrect][tokenCorrect] =
							nb[tokenIncorrect][tokenCorrect] / total[tokenCorrect];
				});

		executor.run();

		printf("Iteration %03d/%03d : OK !\n", i, nbIterations);
	}

	for (auto & it : table)
		for (auto & it2 : it)
			if (it2 >= std::max(TranslationTable::minimalProb, initValueForTable))
				it2 = -log(it2);
			else
				it2 = -1;
}

void TranslationTable::print(FILE * output)
{
	for (unsigned int i = 0; i < table.size(); i++)
		for (unsigned int j = 0; j < table[i].size(); j++)
			if (table[i][j] >= 0)
				fprintf(output, "%u %u %02.9f\n", i, j, table[i][j].load());
}

void TranslationTable::printForDebug(FILE * output, Lexicon & correctLex,
	Lexicon & incorrectLex)
{
	for (unsigned int i = 0; i < table.size(); i++)
		for (unsigned int j = 0; j < table[i].size(); j++)
			if (table[i][j] >= 0)
				fprintf(output, "%02.9f\t<%s> -> <%s>\n", table[i][j].load(),
					incorrectLex.getString(i).c_str(),
					correctLex.getString(j).c_str());
}

void TranslationTable::read(File & input)
{
	table.clear();
	unsigned int token1, token2;
	unsigned int maxToken1 = 0, maxToken2 = 0;
	float proba;

	while (fscanf(input.getDescriptor(), "%u %u %f", &token1, &token2, &proba) == 3)
	{
		if (token1 > maxToken1)
			maxToken1 = token1;

		if (token2 > maxToken2)
			maxToken2 = token2;
	}

	table.resize(maxToken1+1);
	for (auto & it : table)
		it.resize(maxToken2+1);

	for (auto & it : table)
		for (auto & it2 : it)
			it2 = -1;

	input.rewind();

	while (fscanf(input.getDescriptor(), "%u %u %f", &token1, &token2, &proba) == 3)
		table[token1][token2] = proba;
}

void TranslationTable::addTranslations(WordTranslations & actual)
{
	unsigned int token = actual.token;

	if (token >= table.size())
		return;

	for (unsigned int i = 0; i < table[token].size(); i++)
		if (table[token][i] >= 0)
			actual.addTranslation(i, table[token][i]);
}

bool TranslationTable::isCorrect(unsigned int token)
{
	return token <= maxTokenCorrect;
}

