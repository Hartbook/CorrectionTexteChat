/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef EXECUTOR__H
#define EXECUTOR__H

#include <functional>
#include <thread>
#include <vector>

/////////////////////////////////////////////////////////////////////////////
/// \brief Concurrent execution of tasks using fixed threads pool.
///
/// Similar to Executor<void,void> but here functions to execute must have
/// a parameter and return something.
///
/// \tparam ReturnType Return type of tasks to perform. Not void.
/// \tparam ArgType Type of the parameter the tasks take. Not void.
///
/// \see Executor<void, void>
///
////////////////////////////////////////////////////////////////////////////
template <typename ReturnType, typename ArgType>
class Executor
{
	private :

	struct Task
	{
		std::function<ReturnType(ArgType)> run;

		ArgType argument;
		ReturnType result;

		Task(const std::function<ReturnType(ArgType)> & run, ArgType argument) : run(run)
		{
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

	Executor(int nbThreads = std::thread::hardware_concurrency())
	{
		if (nbThreads == 0)
			nbThreads = 1;

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

	void clear()
	{
		threads.clear();
		tasks.clear();
		taskGroups.clear();
	}
};

/////////////////////////////////////////////////////////////////////////////
/// \brief Concurrent execution of tasks using fixed threads pool.
///
/// Usage example :
/// \code
/// // Increment atomic variable using 8 threads
/// const unsigned int nbThreads = 8;
/// Executor<void,void> executor(nbThreads);
/// 
/// std::atomic<int> value = 0;
///
/// for (unsigned int i = 0; i < 1000; i++)
/// 	executor.addTask([&value]()
///		{
/// 		for (unsigned int i = 0; i < 10; i++)
///				value++;
///		});
///
/// executor.run(); // Blocking until every task has been completed.
/// \endcode
///
////////////////////////////////////////////////////////////////////////////
template <>
class Executor<void, void>
{
	private :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief A function without parameter nor return value.
	///
	////////////////////////////////////////////////////////////////////////////
	struct Task
	{
		std::function<void(void)> run;

		Task(const std::function<void(void)> & run) : run(run)
		{
		}
	};

	private :

	int nbThreads;
	std::vector<std::thread> threads;
	std::vector<Task> tasks;
	std::vector< std::vector<Task *> > taskGroups;

	using It = typename std::vector<Task>::iterator;
	/////////////////////////////////////////////////////////////////////////////
	/// \brief run `numberOfTasks` Task starting from `firstTask`
	///
	/// Each thread call this function one time, to run every task assigned to it.
	///
	/// \param firstTask Iterator to the first Task to run.
	/// \param numberOfTasks Number of Task to run.
	///
	/// \see run
	///
	////////////////////////////////////////////////////////////////////////////
	static void runTasks(It firstTask, int numberOfTasks)
	{
		for (int i = 0; i < numberOfTasks; i++)
			(firstTask + i)->run();
	}

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Constructor.
	///
	/// \param nbThreads Number of threads to use, by default it uses every thread
	/// of the machine.
	///
	////////////////////////////////////////////////////////////////////////////
	Executor(int nbThreads = std::thread::hardware_concurrency())
	{
		if (nbThreads == 0)
			nbThreads = 1;

		this->nbThreads = nbThreads;
	}

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Add a Task to perform.
	///
	/// \param func A function that will be ran when run() is called.
	///
	////////////////////////////////////////////////////////////////////////////
	void addTask(std::function<void(void)> func)
	{
		tasks.emplace_back(func);
	}

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Run every Task.
	///
	/// Task are equally distributed between the nbThreads threads, then ran.
	/// This function return only when every Task has been ran.
	///
	////////////////////////////////////////////////////////////////////////////
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

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Delete threads and tasks.
	///
	/// Call this function before adding new Task to the executor if
	/// run() has already been called.
	///
	////////////////////////////////////////////////////////////////////////////
	void clear()
	{
		threads.clear();
		tasks.clear();
		taskGroups.clear();
	}
};

#endif
