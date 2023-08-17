#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

// semaphore struct with count used to call semup and semdown and a queue of the blocked threads waiting on the resource sem is blocking
struct semaphore
{
	size_t count; // internal count
	queue_t blocked_threads; // queue of our waiting threads
};

// initializes the semaphore and queue of blocked threads
sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(struct semaphore));
	// error check if mem alloc fails
	if (sem == NULL)
	{
		return NULL;
	}
	// create queue of threads and initialize count
	preempt_disable();
	sem->blocked_threads = queue_create();
	sem->count = count;
	preempt_enable();
	return sem;
}

// empties the queue with queue_destroy() and frees all related memory
int sem_destroy(sem_t sem)
{
	// error check for sem null or threads still being blocked
	if (sem == NULL || queue_length(sem->blocked_threads) >= 1)
	{
		return -1;
	}
	// destroy queue and deallocate memory of sem
	preempt_disable();
	queue_destroy(sem->blocked_threads);
	free(sem);
	preempt_enable();
	return 0;
}

// decrements semaphore count if already 0 blocks the thread instead
int sem_down(sem_t sem)
{
	// error check if sem is null
	if (sem == NULL)
	{
		return -1;
	}
	preempt_disable();
	while (sem->count == 0)
	{
		// enqueue curr into blocked_thread queue and then block it
		queue_enqueue(sem->blocked_threads, uthread_current());
		uthread_block();
	}
	sem->count--; // change count to be 0 now because down
	preempt_enable();
	return 0;
}

// increments semaphore count if already > 0 unblocks the thread instead
int sem_up(sem_t sem)
{
	if (sem == NULL)
	{
		return -1;
	}
	preempt_disable();
	struct uthread_tcb* unblocked_thread;
	// error check if sem is null
	sem->count++; // count is 1
	// update the waitinglist and unblock the blocked queue
	if (queue_length(sem->blocked_threads) > 0)
	{
		queue_dequeue(sem->blocked_threads, (void**)&unblocked_thread);
		uthread_unblock(unblocked_thread);
	}
	preempt_enable();
	return 0;
}