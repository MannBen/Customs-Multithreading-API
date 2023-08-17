#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

// process states
#define READY 1
#define RUNNING 2
#define BLOCKED 3
#define ZOMBIE 4

queue_t thread_lib; // our queue
struct uthread_tcb* curr_thread = NULL; // our current thread in our queue
struct uthread_tcb* prev_thread = NULL; // our previous thread for our queue
struct uthread_tcb* next_thread = NULL; // our next thread in our queue
int TID = -1; // thread id, the first ID will be 0, we add one to the TID everytime, so start at -1

// struct for holding a given thread's context, tid, state, stack
struct uthread_tcb
{
	uthread_ctx_t* context; // thread will have a context
	int thread_id; // a thread id so each thread is unique
	int state; // ready = 1 , running = 2 , blocked = 3 , zombie = 4
	void *new_stack;
};

// this function will set the thread ID
int set_TID()
{
	TID++;
	return TID;
}

// returns curr_thread	
struct uthread_tcb *uthread_current(void)
{
	// this function will return our global current thread variable
	return curr_thread;
}

// yields out of curr_thread into next READY thread in thread_lib
void uthread_yield(void)
{
	struct uthread_tcb* current = curr_thread;
	preempt_disable();
	
	if(current->state == RUNNING)
	{
		current->state = READY; // change state of the thread being switched out of to READY from RUNNING
		queue_enqueue(thread_lib, curr_thread); // add the thread that was RUNNING to READY queue

	}
	
	prev_thread = current; // set prev thread to current
	queue_dequeue(thread_lib, (void**) &next_thread); // get the oldest thread
	curr_thread = next_thread;
	current = next_thread;
	current->state = RUNNING; // current thread is now running
	
	uthread_ctx_switch(prev_thread->context, current->context); // use private context.c function to switch contexts

	// check if state is Zombie 
	if(current->state == ZOMBIE)
	{
		free(current);
	}	
	preempt_enable();
}

// called at the end of a thread's context in that it reached the end of its code, marks curr_thread as zombie and frees current thread context and stack
void uthread_exit(void)
{
	preempt_disable();
	// deallocation of curr_thread
	free(curr_thread->context);
	uthread_ctx_destroy_stack(curr_thread->new_stack);
	curr_thread->state = ZOMBIE; // thread is finished with execution so change to zombie state
	uthread_yield(); // check if more threads to be run
	preempt_enable();
}

// creates a new thread struct and enqueues it if no errors occurred
int uthread_create(uthread_func_t func, void *arg)
{
	struct uthread_tcb *new_tcb = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	new_tcb->new_stack = uthread_ctx_alloc_stack();
	// check if stack and newtcb was correctly allocated
	if (new_tcb->new_stack == NULL || new_tcb == NULL)
	{
		return -1;
	}
	new_tcb->context = malloc(sizeof(uthread_ctx_t));
	// create thread and add to queue
	if (uthread_ctx_init(new_tcb->context, new_tcb->new_stack, func, arg) == 0)
	{
		new_tcb->state = READY;
		new_tcb->thread_id = set_TID();
		preempt_disable();
		queue_enqueue(thread_lib, new_tcb); //add new_tcb to queue
		preempt_enable();
	}
	else
	{
		return -1;
	}
	return 0;
}

// initializes the idle thread tcb struct as the user's current process and creates the first user-level thread, sets up current process as an idle thread with while(READY THREADS IN QUEUE >= 1)
int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	preempt_start(preempt);
	preempt_disable();
	thread_lib = queue_create(); // create the READY thread_lib queue
	preempt_enable();

	// initialize the stack, context and set state and tid of idle thread which is the current user process
	struct uthread_tcb *tcb_idle = (struct uthread_tcb*)malloc(sizeof(struct uthread_tcb));
	tcb_idle->context = malloc(sizeof(uthread_ctx_t));
	tcb_idle->state = RUNNING; // idle state is now running
	tcb_idle->thread_id = set_TID();
	preempt_disable();
	queue_enqueue(thread_lib , tcb_idle); // add the idle thread to queue as we're about to set it to READY and switch into the first user-level thread

	preempt_enable();
	// call create thread
	if (uthread_create(func, arg) != 0)
	{
		return -1;
	}
	preempt_disable();
 	queue_dequeue(thread_lib, (void**) &curr_thread); // this will assign the curr_thread to be the idle thread
	preempt_enable();

	// this while loop will continue until there are no more threads in our queue
	while (queue_length(thread_lib) >= 1)
	{
		uthread_yield();
	}
	if(preempt) {
		preempt_stop();
	}
	// free the idle thread at end of user-level thread program
	free(tcb_idle->context);
	free(tcb_idle->new_stack);
	free(tcb_idle);
	free(thread_lib);
	return 0;
}

// sets current state to be BLOCKED and yields into new thread, called from sem.c
void uthread_block(void)
{
	preempt_disable();
	curr_thread->state = BLOCKED; // change to blocked state
	uthread_yield();
	preempt_enable();
}

// sets current state to be READY and adds it to READY queue at the back, called from sem.c
void uthread_unblock(struct uthread_tcb *uthread)
{
	// change from blocked state to ready
	preempt_disable();
	if (uthread->state == BLOCKED)
	{
		uthread->state = READY;
	}
	queue_enqueue(thread_lib, uthread);
	preempt_enable();
}