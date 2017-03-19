#include <cstdio>
#include "Executor.hpp"

int main()
{
	int nbTimes = 1000;

	Executor<void, void> executor(2);

	for (int i = 0; i < nbTimes; i++)
		executor.addTask([&](){});

	executor.run();

	return 0;
}

