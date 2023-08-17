/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

#include <uthread.h>
int booleantest = 1;
void thread3(void *arg)
{
	(void)arg;

	uthread_yield();
	booleantest = 0;
	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread2\n");
	sleep(5);
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	uthread_yield();
	while (booleantest)
	{
		printf("thread1");
	}
	printf("thread1 escaped\n");
	uthread_yield();
}

int main(void)
{
	uthread_run(true, thread1, NULL); // makes idle and then thread1 and switch into thread 1
	// thread 3, 2, 1
	return 0;
}
