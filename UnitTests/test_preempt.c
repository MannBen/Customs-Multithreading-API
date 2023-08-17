

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <uthread.h>
bool testBool = true; //used to test preemption out of while
void thread2(void *arg){
    (void)arg;
    testBool = false;
}
void hello(void *arg)
{
	printf("Hello world initial\n");
	(void)arg;
	int itterNum = 0;
    uthread_create(thread2, NULL);
	while (testBool) //while true until preemption
	{
		printf("Hello world!\titeration on Num: %d\n",itterNum);
		itterNum++;
	}
	printf("Hello world escaped through preempt!\n");
}

int main(void)
{
	uthread_run(true, hello, NULL);
	//thread create
	return 0;
}
