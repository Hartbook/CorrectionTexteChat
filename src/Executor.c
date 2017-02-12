#include "Executor.h"
#include "stdio.h"

Task Task_new(void *(*run)(void *), void * argument, void * result)
{
	Task task = {run, argument, result};

	return task;
}

void Task_run(Task * task)
{
	task->result = task->run(task->argument);
}

void * Tasks_run(void * tasks)
{
	int nbTasks = ((Tasks*)tasks)->nbTasks;

	for (int i = 0; i < nbTasks; i++)
		Task_run(((Tasks*)tasks)->tasks +i);

	return NULL;
}

Executor * Executor_new(int nbThreads)
{
	Executor * e = malloc(sizeof *e);

	e->nbThreads = nbThreads;

	for (int i = 0; i < e->nbThreads; i++)
		sprintf(e->threads[i].name, "Thread %d", i);

	e->nbTasks = 0;
	e->nbTasksMax = NB_TASKS_INCREMENT;

	e->tasks = malloc(e->nbTasksMax * sizeof *e->tasks);

	return e;
}

void Executor_delete(Executor * e)
{
	free(e->tasks);
	free(e);
}

void Executor_addTask(Executor * e, Task task)
{
	e->nbTasks++;

	if (e->nbTasks >= e->nbTasksMax)
	{
		e->nbTasksMax += NB_TASKS_INCREMENT;
		e->tasks = realloc(e->tasks, e->nbTasksMax * sizeof *e->tasks);
	}

	e->tasks[e->nbTasks-1] = task;
}

void Executor_run(Executor * e)
{
	int nbTasksPerThread = e->nbTasks / e->nbThreads;
	int nbOrphanTasks = e->nbTasks % e->nbThreads;
	Task * tasks = e->tasks;

	for (int i = 0; i < e->nbThreads; i++)
	{
		int nbTasksToRun = nbTasksPerThread + (i < nbOrphanTasks ? 1 : 0);

		e->taskGroups[i].tasks = tasks;
		e->taskGroups[i].nbTasks = nbTasksToRun;

		if (pthread_create(&e->threads[i].thread, NULL, Tasks_run, e->taskGroups + i))
		{
			char * name = e->threads[i].name;
			printf("Error creating %s\n", name);
		}

		tasks += nbTasksToRun;
	}

	for (int i = 0; i < e->nbThreads; i++)
		pthread_join(e->threads[i].thread, NULL);
}

