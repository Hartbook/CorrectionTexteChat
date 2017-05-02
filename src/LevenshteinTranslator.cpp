#include "LevenshteinTranslator.hpp"
#include <cassert>

LevenshteinTranslator::LevenshteinTranslator(Lexicon & correctLexicon, Lexicon & incorrectLexicon) : correctLexicon(correctLexicon), incorrectLexicon(incorrectLexicon)
{
	order = 1;
}

void LevenshteinTranslator::addTranslations(std::vector< std::pair<unsigned int, float> >&actual,
											unsigned int token)
{
	static auto abs = [](int value) {return value < 0 ? -value : value;};
	static float threshold = 2.0;
	static int diffSizeMax = 2;
	translations.clear();

	const std::string & incorrectWord = incorrectLexicon.getString(token);

	auto & correctWords = correctLexicon.getTokens();
	float proximity;
	for (auto & it : correctWords)
		if (abs(incorrectWord.size() - it.first.size()) <= diffSizeMax)
			if ((proximity = getProximity(incorrectWord, it.first)) <= threshold)
				actual.push_back(std::pair<unsigned int, float>(it.second, proximity));
}

float LevenshteinTranslator::getSubstitutionCost(const std::string & s1, const std::string & s2,
												 unsigned int i, unsigned int j)
{
	if (s1[i] == s2[j])
		return 0.0;

	return 1.0;
}

float LevenshteinTranslator::getProximity(const std::string & s1, const std::string & s2)
{
	distances.resize(s1.size()+1);

	for (unsigned int i = 0; i <= s1.size(); i++)
	{
		distances[i].resize(s2.size()+1);
		distances[i][0] = i;
	}

	for (unsigned int i = 0; i <= s2.size(); i++)
		distances[0][i] = i;

	float subCost = 0.0;

	for (unsigned int i = 1; i <= s1.size(); i++)
		for (unsigned int j = 1; j <= s2.size(); j++)
		{
			subCost = getSubstitutionCost(s1, s2, i-1, j-1);

			distances[i][j] = std::min(distances[i-1][j]+1, distances[i][j-1]+1);
			distances[i][j] = std::min(distances[i][j], distances[i-1][j-1]+subCost);
		}

	return distances.back().back();
}

