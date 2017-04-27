#include "Viterbi.hpp"
#include <algorithm>
#include "util.hpp"
#include "Tokenizer.hpp"

Viterbi::Viterbi(Database & database) : database(database)
{
	translators.emplace_back(& database.translationTable);
}

const std::vector<unsigned int> & Viterbi::correctSentence(
	const std::vector<unsigned int> & sentence)
{
	static std::vector<unsigned int> correctSentence;
	correctSentence.clear();

	using Pair = std::pair<unsigned int, unsigned int>;

	struct Trio
	{
		Pair trad; float proba; int parent;
		Trio(Pair trad, float proba, int parent) :
			trad(trad),
			proba(proba),
			parent(parent){}
	};
	
	static std::vector< std::vector<Trio> > probas;
	probas.clear();
	probas.resize(sentence.size());

	auto & gramsCounter = database.gramsCounter;
	auto & lexicon = database.incorrectLexicon;

	std::vector< std::pair<unsigned int, float> > precedent = {{0,0}}, actual;

	for (unsigned int i = 0; i < sentence.size(); i++)
	{
		actual.clear();
		unsigned int token = sentence[i];

		if (token <= database.correctLexicon.getMaxToken())
		{
			actual.emplace_back(token, 0.0f);
		}
		else
		{
			for (auto translator : translators)
			{
				const auto & translations = translator->getTranslations(token);
				
				for (auto & p : *translations)
				{
					unsigned int trad = p.first;
					float proba = p.second;

					actual.emplace_back(trad, proba);
				}
			}

			if (probas[i].empty())
				actual.emplace_back(token, 0.0f);
		}

		for (auto & prec : precedent)
			for (auto & act : actual)
				probas[i].emplace_back(Pair(prec.first, act.first), act.second, -1);

		precedent = actual;
	}

	for (auto & p : probas[0])
		p.proba += gramsCounter.getLogProb(p.trad.second);

	if (probas.size() == 1)
	{
		int minIndex = 0;

		for (unsigned int i = 1; i < probas[0].size(); i++)
			if (probas[0][i].proba < probas[0][minIndex].proba)
				minIndex = i;

		correctSentence.push_back(probas[0][minIndex].trad.second);

		return correctSentence;
	}

	for (auto & p : probas[1])
	{
		int minIndex = -1;
		float minProba = -1;

		for (unsigned int i = 0; i < probas[0].size(); i++)
			if (p.trad.first == probas[0][i].trad.second)
				if (minIndex == -1 || probas[0][i].proba+gramsCounter.getLogProb(probas[0][i].trad.second, p.trad.second) < minProba)
				{
					minIndex = i;
					minProba = probas[0][i].proba+gramsCounter.getLogProb(probas[0][i].trad.second, p.trad.second);
				}

		p.proba += minProba;
		p.parent = minIndex;
	}

	for (unsigned int j = 2; j < probas.size(); j++)
		for (auto & p : probas[j])
		{
			int minIndex = -1;
			float minProba = -1;

			for (unsigned int i = 0; i < probas[j-1].size(); i++)
				if (p.trad.first == probas[j-1][i].trad.second)
					if (minIndex == -1 || probas[j-1][i].proba+gramsCounter.getLogProb(probas[j-1][i].trad.first, probas[j-1][i].trad.second, p.trad.second) < minProba)
					{
						minIndex = i;
						minProba = probas[j-1][i].proba+gramsCounter.getLogProb(probas[j-1][i].trad.first, probas[j-1][i].trad.second, p.trad.second);
					}

			p.proba += minProba;
			p.parent = minIndex;
		}

	int minIndex = 0;
	float minProba = probas.back()[0].proba;

	for (unsigned int i = 1; i < probas.back().size(); i++)
		if (probas.back()[i].proba < minProba)
		{
			minIndex = i;
			minProba = probas.back()[i].proba;
		}

	correctSentence.push_back(probas.back()[minIndex].trad.second);
	int parent = probas.back()[minIndex].parent;

	for (int i = (int)probas.size()-2; i >= 0; i--)
	{
		correctSentence.push_back(probas[i][parent].trad.second);
		parent = probas[i][parent].parent;
	}

	std::reverse(correctSentence.begin(), correctSentence.end());

	return correctSentence;
}

File * Viterbi::correct(std::string inputFilename)
{
	static const char * pathToTemp = "data/corpus/temp/";

	auto & lexicon = database.incorrectLexicon;

	File * result = new File("data/output/" + getFilenameFromPath(inputFilename) + ".corrected", "w");

	Tokenizer tokenizer(lexicon);

	File * input = new File(inputFilename, "r");

	File * cleanInput = cleanCorpus(input, pathToTemp);

	delete input;

	std::string tempFilename = pathToTemp + getFilenameFromPath(inputFilename) + ".tokenized";

	File * tokenized = tokenizer.tokenize(*cleanInput, tempFilename);

	delete cleanInput;

	std::vector<unsigned int> sentence;

	while (!tokenized->isFinished())
	{
		sentence.emplace_back();

		fscanf(tokenized->getDescriptor(), "%d", & sentence.back());

		if (tokenized->peek() == ' ')
			tokenized->getChar();

		if (endSentence(tokenized->peek()))
		{
			auto & correctedSentence = correctSentence(sentence);
			sentence.clear();

			for (unsigned int token : correctedSentence)
				fprintf(result->getDescriptor(), "%s ", lexicon.getString(token).c_str());

			fprintf(result->getDescriptor(), "\n");
		}
	}

	delete tokenized;

	return result;
}

