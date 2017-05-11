#include "TranslationTable.hpp"
#include "Executor.hpp"
#include <algorithm>

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

			if (fscanf(file.getDescriptor(), "%u", &token) != 1)
				break;

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
					correct[j] = correct.back();

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

	using Pair = std::pair<unsigned int, unsigned int>;

	while (!incorrect.isFinished() && !correct.isFinished())
	{
		correctSentences.emplace_back();
		incorrectSentences.emplace_back();

		readSentence(incorrectSentences.back(), tokensIncorrect, incorrect);
		readSentence(correctSentences.back(), tokensCorrect, correct);

		cleanSentences(correctSentences.back(), incorrectSentences.back());
	}

	float initValueForTable = 1.0 / (incorrectLexicon.getMaxToken()+1);

	table.reserve(tokensCorrect.size() * tokensIncorrect.size());

	for (unsigned int t1 : tokensIncorrect)
		for (unsigned int t2 : tokensCorrect)
			table[Pair(t1,t2)] = initValueForTable;

	unsigned int nbSentences = std::max(correctSentences.size(), incorrectSentences.size());

	std::vector<AtomicFloat> total(correctLexicon.getMaxToken()+1);

	std::unordered_map<Pair, AtomicFloat> nb = table;

	Executor<void,void> executor;

	for (int i = 0; i < nbIterations; i++)
	{
		executor.clear();
	
		for (auto & it : nb)
			it.second = 0.0f;

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
							s_total[tokenIncorrect] +=
								table.at(Pair(tokenIncorrect,tokenCorrect));

				for (auto tokenIncorrect : sentenceIncorrect)
					if (!isCorrect(tokenIncorrect))
						for (auto tokenCorrect : sentenceCorrect)
						{
							Pair p(tokenIncorrect, tokenCorrect);

							nb.at(p) += table.at(p) / s_total[tokenIncorrect];
							total[tokenCorrect] += table.at(p) / s_total[tokenIncorrect];
						}
			});

		executor.run();

		executor.clear();

		for (auto tokenIncorrect : tokensIncorrect)
			if (!isCorrect(tokenIncorrect))
				executor.addTask([this, &nb, &total, &tokensCorrect, tokenIncorrect]()
				{
					for (auto tokenCorrect : tokensCorrect)
					{
						Pair p(tokenIncorrect, tokenCorrect);
						table.at(p) = nb.at(p) / total[tokenCorrect];
					}
				});

		executor.run();

		printf("Iteration %03d/%03d : OK !\n", i+1, nbIterations);
	}

	for (auto & it : table)
		if (it.second >= std::max(TranslationTable::minimalProb, initValueForTable))
			it.second = -log(it.second);
		else
			it.second = -1;

	arrangeTranslations();
}

void TranslationTable::print(FILE * output)
{
	for (auto & it : table)
		if (it.second >= 0)
			fprintf(output, "%u %u %02.9f\n", it.first.first, it.first.second, it.second.load());
}

void TranslationTable::printForDebug(FILE * output, Lexicon & correctLex,
	Lexicon & incorrectLex)
{
	using Pair = std::pair<const std::string*, const std::string*>;
	using PairOfPair = std::pair<Pair, float>;
	std::vector<PairOfPair> sorted;
	sorted.reserve(table.size());

	for (auto & it : table)
		if (it.second >= 0)
		{
			const std::string * s1 = &incorrectLex.getString(it.first.first);
			const std::string * s2 = &correctLex.getString(it.first.second);
			sorted.emplace_back(Pair(s1,s2), it.second);
		}

	std::sort(sorted.begin(), sorted.end(),
		[](const PairOfPair & a, const PairOfPair & b)
		{
			if (*a.first.first != *b.first.first)
				return *a.first.first < *b.first.first;

			return a.second < b.second;
		});

	for (auto & it : sorted)
		fprintf(output, "%02.9f\t<%s> -> <%s>\n",
			it.second,
			it.first.first->c_str(),
			it.first.second->c_str());
}

void TranslationTable::read(File & input)
{
	unsigned int token1, token2;
	float proba;

	while (fscanf(input.getDescriptor(), "%u %u %f", &token1, &token2, &proba) == 3)
		table[std::pair<unsigned int, unsigned int>(token1, token2)] = proba;

	arrangeTranslations();
}

void TranslationTable::addTranslations(WordTranslations & actual)
{
	unsigned int token = actual.token;

	const auto & it = translations.find(token);

	if (it == translations.end())
		return;

	for (auto & translation : it->second)
	actual.addTranslation(translation.first, translation.second);
}

bool TranslationTable::isCorrect(unsigned int token)
{
	return token <= maxTokenCorrect;
}

void TranslationTable::arrangeTranslations()
{
	for (auto & it : table)
		translations[it.first.first].emplace_back(it.first.second, it.second);
}

