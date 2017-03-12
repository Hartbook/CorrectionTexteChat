#ifndef EXECUTOR__H
#define EXECUTOR__H

#include <functional>
#include <thread>
#include <vector>

template <typename ReturnType, typename ArgType>
class Executor
{
	private :

	struct Task
	{
		std::function<ReturnType(ArgType)> run;

		ArgType argument;
		ReturnType result;

		Task(std::function<ReturnType(ArgType)> run, ArgType argument)
		{
			this->run = run;
			this->argument = argument;
		}
	};

	private :

	int nbThreads;
	std::vector<std::thread> threads;
	std::vector<Task> tasks;
	std::vector< std::vector<Task *> > taskGroups;

	using It = typename std::vector<Task>::iterator;
	static void runTasks(It firstTask, int numberOfTasks)
	{
		for (int i = 0; i < numberOfTasks; i++)
			(firstTask + i)->result = (firstTask + i)->run((firstTask + i)->argument);
	}

	public :

	Executor(int nbThreads)
	{
		this->nbThreads = nbThreads;
	}

	void addTask(std::function<ReturnType(ArgType)> func, ArgType argument)
	{
		tasks.emplace_back(func, argument);
	}

	void run()
	{
		int nbTasksPerThread = tasks.size() / nbThreads;
		int nbOrphanTasks = tasks.size() % nbThreads;
		auto it = tasks.begin();

		for (int i = 0; i < nbThreads; i++)
		{
			int nbTasksToRun = nbTasksPerThread + (i < nbOrphanTasks ? 1 : 0);

			threads.emplace_back(runTasks, it, nbTasksToRun);

			it += nbTasksToRun;
		}

		for (auto & t : threads)
			t.join();
	}
};

#endif
