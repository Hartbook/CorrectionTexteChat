#ifndef EXECUTOR__H
#define EXECUTOR__H

#include <pthread.h>
#include <malloc.h>

#define _REENTRANT
#define MAX_NB_THREADS 16
#define NB_TASKS_INCREMENT 64

#define Task(a,b,c) Task_new((void * (*)(void*))a, b, c)

typedef struct Task
{
	void * (*run)(void *);

	void * argument;
	void * result;
} Task;

typedef struct Tasks
{
	Task * tasks;
	int nbTasks;
} Tasks;

typedef struct Thread
{
	pthread_t thread;
	char name[16];
} Thread;

typedef struct Executor
{
	Thread threads[MAX_NB_THREADS];
	int nbThreads;

	Task * tasks;
	int nbTasks;
	int nbTasksMax;

	Tasks taskGroups[MAX_NB_THREADS];
} Executor;

Task Task_new(void *(*run)(void *), void * argument, void * result);

Executor * Executor_new(int nbThreads);
void Executor_delete(Executor * e);
void Executor_addTask(Executor * e, Task task);
void Executor_run(Executor * e);

#endif
