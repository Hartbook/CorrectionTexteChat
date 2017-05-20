#include "Viterbi.hpp"
#include <algorithm>
#include <cstring>
#include "util.hpp"
#include "Tokenizer.hpp"
#include "Executor.hpp"

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

void Viterbi::buildLatticeFromSentence(std::vector< std::vector<Trio> > & probas, const std::vector<unsigned int> & sentence)
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

void Viterbi::computeViterbiForRow(std::vector< std::vector<Trio> > & probas, unsigned int row)
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

//		gram *= gram; // Boost importance of n-grams model

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

void Viterbi::correctSentence(std::vector<unsigned int> & dest, const std::vector<unsigned int> & sentence)
{
	std::vector< std::vector<Trio> > probas;
	probas.resize(sentence.size());

	buildLatticeFromSentence(probas, sentence);

	for (auto & p : probas[0])
		p.proba += gramsCounter.getLogProb(p.trad.second);

	if (probas.size() == 1)
	{
		int minIndex = 0;

		for (unsigned int i = 1; i < probas[0].size(); i++)
			if (probas[0][i].proba < probas[0][minIndex].proba)
				minIndex = i;

		dest.push_back(probas[0][minIndex].trad.second);

		return;
	}

	for (unsigned int j = 1; j < probas.size(); j++)
		computeViterbiForRow(probas, j);

	int minIndex = 0;
	float minProba = probas.back()[0].proba;

	for (unsigned int i = 1; i < probas.back().size(); i++)
		if (probas.back()[i].proba < minProba)
		{
			minIndex = i;
			minProba = probas.back()[i].proba;
		}

	dest.push_back(probas.back()[minIndex].trad.second);
	int parent = probas.back()[minIndex].parent;

	for (int i = (int)probas.size()-2; i >= 0; i--)
	{
		dest.push_back(probas[i][parent].trad.second);
		parent = probas[i][parent].parent;
	}

	std::reverse(dest.begin(), dest.end());

	nbCorrected++;
	printf("\rNb sentences corrected : %06d / %06d (%4f%%)", nbCorrected.load(), nbTotal, (100.0*nbCorrected.load() / nbTotal));
	fflush(stdout);

	//printLatticeForDebug(probas);
	// Can be useful to see how the correction was obtained
}

File * Viterbi::correct(std::string inputFilename)
{
	nbCorrected = 0;
	static const char * pathToTemp = "data/corpus/temp/";

	auto & lexicon = database.incorrectLexicon;

	File * result = new File("data/output/" + getFilenameFromPath(inputFilename) + ".corrected", "w");

	Tokenizer tokenizer(lexicon);

	File * input = new File(inputFilename, "r");
	File * cleanInput = new File(pathToTemp + getFilenameFromPath(inputFilename), "w");

	cleanCorpus(*input, *cleanInput);
	cleanInput->rewind();

	delete input;

	std::string tempFilename = pathToTemp + getFilenameFromPath(inputFilename) + ".tokenized";

	File * tokenized = new File(tempFilename, "w");
	tokenizer.tokenize(*cleanInput, *tokenized);
	tokenized->rewind();

	delete cleanInput;

	auto readSentence = [](std::vector<unsigned int> & sentence, File & file)
	{
		sentence.clear();

		while (!file.isFinished() && file.peek() != '\n')
		{
			unsigned int token;

			if (fscanf(file.getDescriptor(), "%u", &token) != 1)
				break;

			sentence.push_back(token);
			file.getChar();
		}

		file.getChar();
	};

	auto printSentence = [&lexicon](std::vector<unsigned int> & sentence, File & file)
	{
		for (unsigned int token : sentence)
			fprintf(file.getDescriptor(), "%s ", lexicon.getString(token).c_str());

		fprintf(file.getDescriptor(), "\n");
	};

	std::vector< std::vector<unsigned int> > sentences;

	while (!tokenized->isFinished())
	{
		sentences.emplace_back();
		readSentence(sentences.back(), *tokenized);
		if (sentences.back().empty())
			sentences.pop_back();
	}

	delete tokenized;

	nbTotal = sentences.size();

	Executor<void,void> executor;

	for (auto & sentence : sentences)
		executor.addTask([&]()
		{
			std::vector<unsigned int> corrected;
			correctSentence(corrected, sentence);
			sentence = corrected;
		});

	executor.run();

	for (auto & sentence : sentences)
		printSentence(sentence, *result);

	return result;
}


void Viterbi::printLatticeForDebug(std::vector< std::vector<Trio> > & probas)
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

