#include "Database.hpp"
#include "util.hpp"
#include "Tokenizer.hpp"
#include <cstdio>

Database::Database()
{

}

void Database::buildFromCorpus(std::string correctName, std::string incorrectName)
{
	FILE * correct = readFile(correctName);
	FILE * incorrect = readFile(incorrectName);

	char c;
	Tokenizer * tokenizer = new Tokenizer;

	while (c = fgetc(correct) != EOF)
	{
		ungetc(c, correct);
		tokenizer->tokenize(correct);
		correctLexicon.addWord(tokenizer->getCurrentWord());
	}
	
	fclose(correct);

	delete tokenizer;
	tokenizer = new Tokenizer;

	while (c = fgetc(incorrect) != EOF)
	{
		ungetc(c, incorrect);
		tokenizer->tokenize(incorrect);
		incorrectLexicon.addWord(tokenizer->getCurrentWord());
	}

	fclose(incorrect);

	delete tokenizer;
}

