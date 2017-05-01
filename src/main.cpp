#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "Database.hpp"
#include "Viterbi.hpp"
#include "Layout.hpp"

void printUsageAndExit(char * argv[])
{
	printf("USAGE : %s OPTION [args]\n", *argv);
	printf("--buildDatabase correctCorpus incorrectCorpus\n");
	printf("--correct incorrectLexicon correctLexicon gramsCount translationTable input\n");

	exit(1);
}

enum Option
{
	BuildDatabase,
	Correct,
	None
};

Option parseArgs(int argc, char * argv[])
{
	static std::unordered_map<std::string, Option> options =
	{
		{"--buildDatabase", Option::BuildDatabase},
		{"--correct", Option::Correct}
	};

	Option result = Option::None;

	for (int i = 0; i < argc; i++)
	{
		if (options.count(argv[i]))
		{
			if (result != Option::None)
				return Option::None;

			result = options[argv[i]];
		}
	}

	return result;
}

int main(int argc, char * argv[])
{
	if (argc < 2)
		printUsageAndExit(argv);

	Option option = parseArgs(argc, argv);

	if (option == Option::None)
		printUsageAndExit(argv);

	if (option == Option::BuildDatabase)
	{
		if (argc != 4)
			printUsageAndExit(argv);

		std::string correctFilename = argv[2];
		std::string incorrectFilename = argv[3];

		Database database;
		database.buildFromCorpus(correctFilename, incorrectFilename);
	}
	else if (option == Option::Correct)
	{
		if (argc != 7)
			printUsageAndExit(argv);

		std::string incorrectFilename = argv[2];
		std::string correctFilename = argv[3];
		std::string gramsFilename = argv[4];
		std::string tableFilename = argv[5];
		std::string inputFilename = argv[6];

		Database database;
		database.readFromFiles(incorrectFilename, correctFilename,
			gramsFilename, tableFilename);

		Viterbi viterbi(database);

		File * corrected = viterbi.correct(inputFilename);
		File * base = new File(inputFilename, "r");
		File * target = new File(corrected->getName() + ".layout", "w");

		corrected->rewind();
		Layout layout(base, corrected);
		layout.transferLayout(target);

		delete corrected;
		delete base;
		delete target;
	}

	return 0;
}

