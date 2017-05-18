#include "LevenshteinTranslator.hpp"
#include "util.hpp"

LevenshteinTranslator::LevenshteinTranslator(Lexicon & correctLexicon, Lexicon & incorrectLexicon) : correctLexicon(correctLexicon), incorrectLexicon(incorrectLexicon)
{
	order = 1;
}


void LevenshteinTranslator::addTranslations(WordTranslations & actual)
{
	static auto abs = [](int value) {return value < 0 ? -value : value;};
	static float threshold = 2.0;
	static int diffSizeMax = 2;

	const std::string & incorrectWord = incorrectLexicon.getString(actual.token);

	auto & correctWords = correctLexicon.getTokens();
	float proximity;
	for (auto & it : correctWords)
		if (abs(lengthPrinted(incorrectWord) - lengthPrinted(it.first)) <= diffSizeMax)
			if ((proximity = getProximity(incorrectWord, it.first)) <= threshold)
				actual.addTranslation(it.second, 2 + proximity);
}

float LevenshteinTranslator::getSubstitutionCost(const std::string & s1, const std::string & s2,
												 unsigned int i, unsigned int j)
{
	if (s1[i] == s2[j])
		return 0.0f;

	if (s2[j] < 0)
		if (s1[i] == 'a' || s1[i] == 'e' || s1[i] == 'i' || s1[i] == 'o' || s1[i] == 'u')
			return 0.1f;

	if (s2[j] < 0)
		return 1.0f;

	return 1.0f;
}

float LevenshteinTranslator::getAddCost(const std::string & s1, const std::string & s2,
								 unsigned int i, unsigned int j, boolMatrix & addHasOccured)
{
	if (!addHasOccured[i][j] && i == s1.size())
	{
		addHasOccured[i][j] = true;
		return 0.3f;
	}

	if (s2[j] < 0)
		return 0.1f;

	if (s2[j] == '-' || s2[j] == '\'' || s2[j] == '_')
		return 0.1f;

	return 1.0f;
}

float LevenshteinTranslator::getDelCost(const std::string & s1, const std::string & s2,
								 unsigned int i, unsigned int j, boolMatrix & delHasOccured)
{
	if (!delHasOccured[i][j] && j == s1.size())
	{
		delHasOccured[i][j] = true;
		return 0.3f;
	}

	if (s1[i] < 0)
	{
		if (i == 0 && i < s1.size() && s1[i+1] >= 0)
			return 0.0f;
		if (i < s1.size() && s1[i+1] >= 0 && s1[i-1] >= 0)
			return 0.0f;
		if (i == s1.size()-1 && s1[i-1] >= 0)
			return 0.0f;

		return 1.0f;
	}

	if (s1[i] == '-' || s1[i] == '\'' || s1[i] == '_')
		return 0.1f;

	return 1.0f;
}

float LevenshteinTranslator::getProximity(const std::string & s1, const std::string & s2)
{
	std::vector< std::vector<float> > distances;
	boolMatrix delHasOccured;
	boolMatrix addHasOccured;

	distances.resize(s1.size()+1);
	delHasOccured.resize(s1.size()+1);
	addHasOccured.resize(s1.size()+1);

	for (unsigned int i = 0; i <= s1.size(); i++)
	{
		distances[i].resize(s2.size()+1);
		distances[i][0] = i;
		delHasOccured[i].resize(s2.size()+1, false);
		addHasOccured[i].resize(s2.size()+1, false);
		for (unsigned int j = 0; j <= s2.size(); j++)
		{
			delHasOccured[i][j] = false;
			addHasOccured[i][j] = false;
		}
	}

	for (unsigned int i = 0; i <= s2.size(); i++)
		distances[0][i] = i;

	float subCost = 0.0;
	float addCost = 0.0;
	float delCost = 0.0;

	for (unsigned int i = 1; i <= s1.size(); i++)
		for (unsigned int j = 1; j <= s2.size(); j++)
		{
			subCost = getSubstitutionCost(s1, s2, i-1, j-1);
			addCost = getAddCost(s1, s2, i, j-1, addHasOccured);
			delCost = getDelCost(s1, s2, i-1, j, delHasOccured);

			distances[i][j] = std::min(distances[i-1][j]+delCost, distances[i][j-1]+addCost);
			distances[i][j] = std::min(distances[i][j], distances[i-1][j-1]+subCost);
		}

	return distances.back().back();
}

