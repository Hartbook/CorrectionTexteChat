#include "WordTranslator.hpp"

void WordTranslations::initForToken(unsigned int token)
{
	this->token = token;
	translations.clear();
	positions.clear();
}

float WordTranslations::combineProbas(float p1, float p2)
{
	float min = std::min(p1, p2);
	float max = std::max(p1, p2);
	float diff = max - min;

	return min - std::max<double>(0.5*(min-diff*diff), 0.0f);
}

void WordTranslations::addTranslation(unsigned int token, float proba)
{
	if (positions.count(token))
	{
		Pair * p = positions.at(token);

		p->second = combineProbas(p->second, proba);
	}
	else
	{
		translations.emplace_back(new Pair(token, proba));
		positions[token] = translations.back().get();
	}
}

bool WordTranslations::empty()
{
	return translations.empty();
}

