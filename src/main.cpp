#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "Database.hpp"
#include "Viterbi.hpp"
#include "Layout.hpp"

void printUsageAndExit(char * argv[])
{
	printf("USAGE : %s OPTION [args]\n", *argv);
	printf("--buildDatabase -lexicons <List of lexicons>\n\t\
-corpora <Corpora to learn from> -pairs <Pairs of incorrect-correct texts>\n");
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

void parseArgsForBuildDatabase(int argc, char * argv[], std::vector<std::string> & lexicons,
	std::vector<std::string> & corpora, std::vector< std::pair<std::string, std::string> >&pairs)
{
	std::unordered_map< std::string, std::function<void(int)> > options =
	{
		{"-lexicons", [&](int i)
				{
					for (int j = i+1; j < argc; j++)
					{
						if (options.count(argv[j]))
							break;

						lexicons.emplace_back(argv[j]);
					}

					if (lexicons.empty())
						printUsageAndExit(argv);
				}
		},
		{"-corpora", [&](int i)
				{
					for (int j = i+1; j < argc; j++)
					{
						if (options.count(argv[j]))
							break;

						corpora.emplace_back(argv[j]);
					}

					if (corpora.empty())
						printUsageAndExit(argv);
				}
		},
		{"-pairs", [&](int i)
				{
					for (int j = i+1; j < argc; j+=2)
					{
						if (options.count(argv[j]))
							break;
						
						if (options.count(argv[j+1]))
							printUsageAndExit(argv);

						pairs.emplace_back(argv[j], argv[j+1]);
					}

					if (pairs.empty())
						printUsageAndExit(argv);
				}
		},
	};

	int i = 0;
	for (i = 0; i < argc; i++)
	{
		auto it = options.find(argv[i]);

		if (it != options.end())
			it->second(i);
	}
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
		if (argc < 4)
			printUsageAndExit(argv);

		std::vector<std::string> lexicons;
		std::vector<std::string> corpora;
		std::vector< std::pair<std::string, std::string> > pairs;

		parseArgsForBuildDatabase(argc, argv, lexicons, corpora, pairs);

		Database database;
		database.buildFromCorpus(lexicons, corpora, pairs);
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
		Layout layout(base, corrected, database.fusions);
		layout.transferLayout(target);

		delete corrected;
		delete base;
		delete target;
	}

	return 0;
}

