#include "Database.hpp"
#include "File.hpp"
#include "Tokenizer.hpp"
#include <cstdio>

Database::Database()
{

}

void Database::buildFromCorpus(std::string correctName, std::string incorrectName)
{
	File correct(correctName, "r");
	File incorrect(incorrectName, "r");

	Tokenizer * tokenizer1 = new Tokenizer;
	Tokenizer * tokenizer2 = new Tokenizer;

	while (!correct.isFinished())
	{
		tokenizer1->tokenize(correct);
		correctLexicon.addWord(tokenizer1->getCurrentWord());
	}

	for (unsigned int i = 0; i < 20; i++)
		printf("%d <%s>\n", i, correctLexicon.getString(i).c_str());
	
	while (!incorrect.isFinished())
	{
		tokenizer2->tokenize(incorrect);
		incorrectLexicon.addWord(tokenizer2->getCurrentWord());
	}

	delete tokenizer1;
	delete tokenizer2;
}

