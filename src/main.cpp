#include <cstdio>
#include "Executor.hpp"

int main()
{
	int nbTimes = 1000*10;

	Executor<void*, void*> executor(2);

	std::vector<int> dawa;

	for (int i = 0; i < nbTimes; i++)
		executor.addTask([&](void*){printf("TG\n");dawa.push_back(1); dawa.pop_back(); return
			nullptr;}, nullptr);

	executor.run();

	return 0;
}

