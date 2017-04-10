#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "Database.hpp"

void printUsageAndExit(char * argv[])
{
	printf("USAGE : %s OPTION [args]\n", *argv);
	printf("--buildDatabase correctCorpus incorrectCorpus\n");

	exit(1);
}

enum Option
{
	BuildDatabase,
	None
};

Option parseArgs(int argc, char * argv[])
{
	static std::unordered_map<std::string, Option> options =
	{
		{"--buildDatabase", Option::BuildDatabase}
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

	return 0;
}

