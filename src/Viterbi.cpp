#include "Viterbi.hpp"
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
	
	static std::vector< std::vector< std::vector< std::pair<unsigned int, float> > *> > lattice;
	lattice.resize(sentence.size());

	auto & gramsCounter = database.gramsCounter;

	for (unsigned int i = 0; i < sentence.size(); i++)
	{
		unsigned int token = sentence[i];

		for (auto translator : translators)
			lattice[i].push_back(translator->getTranslations(token));
	}

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

