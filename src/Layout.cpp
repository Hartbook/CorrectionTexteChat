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
			   s == Lexicon::dateStr;
	};

	std::string wordCorrect, wordIncorrect;
	char buffer[4096];
	char read = '\n';

	bool mustReadWord = true;

	while (!baseText->isFinished())
	{
		if (mustReadWord)
		{
			mustReadWord = false;
			fscanf(correctedText->getDescriptor(), "%s", buffer);
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
				wordIncorrect.clear();
		}

		fscanf(baseText->getDescriptor(), "%c", &read);

		if (isSeparator(read) || (read == '-' && wordIncorrect.empty()))
		{
			if (!wordIncorrect.empty())
			{
				if (!isSpecial(wordCorrect))
					fprintf(target->getDescriptor(), "%s", wordCorrect.c_str());
				else
					fprintf(target->getDescriptor(), "%s", wordIncorrect.c_str());
				mustReadWord = true;
				wordIncorrect.clear();
			}

			fprintf(target->getDescriptor(), "%c", read);
		}
		else
		{
			wordIncorrect.push_back(read);
		}
	}
}

