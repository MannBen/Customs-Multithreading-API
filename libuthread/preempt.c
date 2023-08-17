#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define TARGETTIME 1000000
#define TARGETSECONDS  0
int test = 0;
sigset_t disable_alarm;
struct itimerval old_timer, new_timer;
struct sigaction sa;

// receive signal from itimer, liked in sigaction sa
void preempt_handler()
{
	uthread_yield(); // forcefully yield to the currently running thread
}

// disables preemption with SIG_BLOCK
void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &disable_alarm, NULL);
}

// enable preemption with SIG_UNBLOCK
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &disable_alarm, NULL);
}

// initializes preemption if the bool passed to uthread_run was true, else return
void preempt_start(bool preempt)
{
	if (preempt) // passed true
	{
		sa.sa_handler = preempt_handler;
		sigemptyset(&disable_alarm);
		sigaddset(&disable_alarm, SIGVTALRM);

		if(sigaction(SIGVTALRM, &sa, NULL)) // setting handler for appropriate alarm signal, catch error
		{
			return;
		}

		new_timer.it_value.tv_usec = TARGETTIME/HZ; // = 10000 microseconds
		new_timer.it_value.tv_sec = TARGETSECONDS; // 0 seconds + 10000 microseconds
		new_timer.it_interval.tv_usec = TARGETTIME/HZ; // = 10000 microseconds
		new_timer.it_interval.tv_sec = TARGETSECONDS; // 0 seconds + 10000 microseconds
		setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer); // set new timer
	}
	else
	{
		return;
	}
	
}

// ends preemption by disabling the timer and sigset_t disable_alarm
void preempt_stop(void)
{
	preempt_disable();
	sigprocmask(SIG_SETMASK, &disable_alarm, NULL); // override disable_alarm with to remove SIGVTALARM
	setitimer(ITIMER_VIRTUAL, &old_timer, NULL); // resets itimer values to old_timer
}