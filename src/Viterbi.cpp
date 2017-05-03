#include "Viterbi.hpp"
#include <algorithm>
#include <cstring>
#include "util.hpp"
#include "Tokenizer.hpp"

Viterbi::Viterbi(Database & database) :
	database(database), lexicon(database.incorrectLexicon), gramsCounter(database.gramsCounter)
{
	translators.emplace_back(& database.translationTable);
	translators.emplace_back(& database.levenshteinTranslator);

	std::sort(translators.begin(), translators.end(),
		[](const WordTranslator * a, const WordTranslator * b)
		{
			return a->order <= b->order;
		});
}

void Viterbi::buildLatticeFromSentence(const std::vector<unsigned int> & sentence)
{
	std::vector< std::pair<unsigned int, float> > precedent = {{0,0}};
	WordTranslations actual;

	for (unsigned int i = 0; i < sentence.size(); i++)
	{
		unsigned int token = sentence[i];
		actual.initForToken(token);

		if (token <= database.correctLexicon.getMaxToken())
		{
			actual.addTranslation(token, 0.0f);
		}
		else
		{
			for (auto translator : translators)
				translator->addTranslations(actual);

			if (actual.empty())
				actual.addTranslation(token, 0.0f);
		}

		for (auto & prec : precedent)
			for (auto & act : actual.translations)
				probas[i].emplace_back(Pair(prec.first, act.get()->first),act.get()->second, -1);

		precedent.clear();
		for (auto & act : actual.translations)
			precedent.push_back(*act.get());
	}
}

void Viterbi::computeViterbiForRow(unsigned int row)
{
	auto getProbaCommingFrom = [&](unsigned int line, Trio & current)
	{
		float previous = probas[row-1][line].proba;
		float gram = 0.0f;

		if (row == 1)
			gram = gramsCounter.getLogProb(probas[row-1][line].trad.second, current.trad.second);
		else
			gram = gramsCounter.getLogProb(probas[row-1][line].trad.first,
				probas[row-1][line].trad.second, current.trad.second);

		return previous + gram;
	};

	for (auto & p : probas[row])
	{
		int minIndex = -1;
		float minProba = -1;

		for (unsigned int i = 0; i < probas[row-1].size(); i++)
			if (p.trad.first == probas[row-1][i].trad.second)
				if (minIndex == -1 || getProbaCommingFrom(i, p) < minProba)
				{
					minIndex = i;
					minProba = getProbaCommingFrom(i, p);
				}

		p.proba += minProba;
		p.parent = minIndex;
	}
}

const std::vector<unsigned int> & Viterbi::correctSentence(
	const std::vector<unsigned int> & sentence)
{
	corrected.clear();
	probas.clear();
	probas.resize(sentence.size());

	buildLatticeFromSentence(sentence);

	for (auto & p : probas[0])
		p.proba += gramsCounter.getLogProb(p.trad.second);

	if (probas.size() == 1)
	{
		int minIndex = 0;

		for (unsigned int i = 1; i < probas[0].size(); i++)
			if (probas[0][i].proba < probas[0][minIndex].proba)
				minIndex = i;

		corrected.push_back(probas[0][minIndex].trad.second);

		return corrected;
	}

	for (unsigned int j = 1; j < probas.size(); j++)
		computeViterbiForRow(j);

	int minIndex = 0;
	float minProba = probas.back()[0].proba;

	for (unsigned int i = 1; i < probas.back().size(); i++)
		if (probas.back()[i].proba < minProba)
		{
			minIndex = i;
			minProba = probas.back()[i].proba;
		}

	corrected.push_back(probas.back()[minIndex].trad.second);
	int parent = probas.back()[minIndex].parent;

	for (int i = (int)probas.size()-2; i >= 0; i--)
	{
		corrected.push_back(probas[i][parent].trad.second);
		parent = probas[i][parent].parent;
	}

	std::reverse(corrected.begin(), corrected.end());

	printLatticeForDebug();

	return corrected;
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

void Viterbi::printLatticeForDebug()
{
	bool finished = false;
	unsigned int index = 0;
	int basePadding = 26;
	char buffer[128];

	while (!finished)
	{
		finished = true;

		for (unsigned int i = 0; i < probas.size(); i++)
		{
			int padding = basePadding;

			if (index < probas[i].size())
			{
				finished = false;

				const std::string & word = lexicon.getString(probas[i][index].trad.second);
				float proba = probas[i][index].proba;
				sprintf(buffer, "%f", proba);
				printf("<%s>(%s)", word.c_str(), buffer);
				padding -= lengthPrinted(word) + strlen(buffer) + 4;
			}

			for (int i = 0; i < padding; i++)
				printf(" ");
		}

		index++;

		printf("\n");
	}

	printf("\n");
}

