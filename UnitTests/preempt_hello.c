/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <uthread.h>

void hello(void *arg)
{
	printf("Hello world initial\n");
	(void)arg;
	int test = 0;
	while (1)
	{
		printf("Hello world!\t%d\n",test);
		test++;
		//sleep(0.5);
	}
	printf("Hello world escaped through preempt!\n");
}

int main(void)
{
	uthread_run(true, hello, NULL);
	//thread create
	return 0;
}
