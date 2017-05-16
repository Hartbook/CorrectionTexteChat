#include "Layout.hpp"
#include "util.hpp"
#include "Lexicon.hpp"

Layout::Layout(File * baseText, File * correctedText)
{
	this->baseText = baseText;
	this->correctedText = correctedText;
}

void Layout::transferLayout(File * target)
{
	auto isNewline = [](char c) {return c == 10 || c == 13;};
	auto isSpecial = [](const std::string & s)
	{
		return s == Lexicon::unknownStr || s == Lexicon::mailStr ||
			   s == Lexicon::numberStr || s == Lexicon::properNounStr ||
			   s == Lexicon::dateStr || s == Lexicon::urlStr || s == Lexicon::newSentenceStr;
	};
	auto removeDashes = [](std::string & s)
	{
		for (unsigned int i = 0; i < s.size(); i++)
			if (s[i] == '_')
			{
				if (i < s.size()-1)
					s[i] = ' ';
				else s.pop_back();
			}
	};

	std::string wordCorrect, wordIncorrect;
	char buffer[4096];
	char read = '\n';

	bool mustReadWord = true;
	bool firstWordOfSentence = true;
	bool incorrectContainsApostrophe = false;

	while (!baseText->isFinished())
	{
		if (mustReadWord)
		{
			mustReadWord = false;
			fscanf(correctedText->getDescriptor(), "%s", buffer);
			if (correctedText->peek() == ' ')
				correctedText->getChar();
			wordCorrect = buffer;
		}

		bool triggered = true;

		while (triggered)
		{
			triggered = false;

			while (isNewline(read) && baseText->peek() == '#')
			{
				triggered = true;

				do
				{
					fscanf(baseText->getDescriptor(), "%c", &read);
					fprintf(target->getDescriptor(), "%c", read);
				} while (!isNewline(read));
			}

			while (isNewline(read) && baseText->peek() == '[')
			{
				triggered = true;

				do
				{
					fscanf(baseText->getDescriptor(), "%c", &read);
					fprintf(target->getDescriptor(), "%c", read);
				} while (read != ']');

				do
				{
					fscanf(baseText->getDescriptor(), "%c", &read);
					fprintf(target->getDescriptor(), "%c", read);
				} while (read != ':' && !isNewline(read));
			}

			if (triggered)
			{
				incorrectContainsApostrophe = false;
				wordIncorrect.clear();
			}
		}

		fscanf(baseText->getDescriptor(), "%c", &read);

		if (isSeparator(read) || (read == '-' && wordIncorrect.empty()))
		{
			if (!wordIncorrect.empty())
			{
				if (!isSpecial(wordCorrect))
				{
					if (firstWordOfSentence)
						toUpperCase(wordCorrect, 0);

					removeDashes(wordCorrect);

					bool correctContainsApostrophe = false;

					for (char c : wordCorrect)
						if (c == '\'')
							correctContainsApostrophe = true;

					if (incorrectContainsApostrophe && !correctContainsApostrophe)
					{
						if (wordIncorrect[0] == '\'')
						{
							wordCorrect.insert(wordCorrect.begin(), '\'');
						}
						else
						{
							wordCorrect.push_back('\'');

							if (wordIncorrect.back() != '\'')
							{
								if (correctedText->peek() == ' ')
									correctedText->getChar();
								while (!isSeparator(correctedText->peek()))
									wordCorrect.push_back(correctedText->getChar());
							}
						}
					}

					fprintf(target->getDescriptor(), "%s", wordCorrect.c_str());
				}
				else
					fprintf(target->getDescriptor(), "%s", wordIncorrect.c_str());

				firstWordOfSentence = false;
				mustReadWord = true;
				incorrectContainsApostrophe = false;
				wordIncorrect.clear();
			}

			fprintf(target->getDescriptor(), "%c", read);
		}
		else
		{
			if (read == '\'')
				incorrectContainsApostrophe = true;
			wordIncorrect.push_back(read);
		}

		if (endSentence(read))
			firstWordOfSentence = true;
	}
}

