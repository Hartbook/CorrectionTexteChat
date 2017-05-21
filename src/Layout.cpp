#include "Layout.hpp"
#include "util.hpp"
#include "Lexicon.hpp"
#include <cstring>

Layout::Layout(File * baseText, File * correctedText, std::vector< std::set<unsigned int> > & fusions)
{
	this->baseText = baseText;
	this->correctedText = correctedText;
	this->fusions = fusions;
}

void Layout::transferLayout(File * target)
{
	auto isSpecial = [](const std::string & s)
	{
		return s == Lexicon::unknownStr || s == Lexicon::mailStr ||
			   s == Lexicon::numberStr || s == Lexicon::properNounStr ||
			   s == Lexicon::dateStr || s == Lexicon::urlStr || s == Lexicon::newSentenceStr;
	};

	auto contains = [](const std::string & s, char c)
	{
		for (char cc : s)
			if (cc == c)
				return true;

		return false;
	};

	bool sentenceBegin;
	std::string incorrectWord;
	std::string correctWord;

	std::vector< std::vector<std::string> > corrected;
	corrected.emplace_back();

	while (!correctedText->isFinished())
	{
		correctWord.clear();
		sentenceBegin = ignoreSeparators(*correctedText);

		if (correctedText->isFinished())
			break;

		if (sentenceBegin)
			corrected.emplace_back();

		readWord(*correctedText, correctWord, sentenceBegin);
		corrected.back().emplace_back(correctWord);
	}

	unsigned int line = 0;
	unsigned int wordIndex = 0;

	bool dropNextWord = false;
	int incorrectIndex = -1;

	bool correctContainsApostrophe = false;
	bool incorrectContainsApostrophe = false;

	unsigned int sentenceSize = getNextSentenceSize(*baseText);

	while (!baseText->isFinished())
	{
		incorrectWord.clear();
		sentenceBegin = ignoreAndPrintSeparators(*baseText, *target);

		if (sentenceBegin)
		{
			line++;
			sentenceSize = getNextSentenceSize(*baseText);
			wordIndex = 0;
			incorrectIndex = 0;
			if (line >= corrected.size())
				break;
		}

		readWord(*baseText, incorrectWord, sentenceBegin);
		incorrectIndex++;

		if ((!dropNextWord || (((unsigned int)incorrectIndex) >= sentenceSize-1)) && wordIndex < corrected[line].size())
		{
			correctContainsApostrophe = contains(corrected[line][wordIndex], '\'');
			incorrectContainsApostrophe = contains(incorrectWord, '\'');

			if (incorrectContainsApostrophe && !correctContainsApostrophe)
			{
				while (incorrectWord.back() != '\'')
				{
					baseText->ungetChar(incorrectWord.back());
					incorrectWord.pop_back();
				}
	
				baseText->ungetChar(incorrectWord.back());
				incorrectWord.pop_back();
			}

			correctWord = corrected[line][wordIndex];

			if (isSpecial(correctWord))
				fprintf(target->getDescriptor(), "%s", incorrectWord.c_str());
			else
				fprintf(target->getDescriptor(), "%s", correctWord.c_str());

			if (fusions[line].find(incorrectIndex) != fusions[line].end())
					dropNextWord = true;

			wordIndex++;
		}
		else
		{
			dropNextWord = false;
			if (wordIndex >= corrected[line].size())
				fprintf(target->getDescriptor(), "%s", incorrectWord.c_str());
		}
	}
}

