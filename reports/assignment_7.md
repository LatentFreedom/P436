Nick Palumbo
Assignment 7 report
Chapters 13-14

## 1.
# a. Trace the series of function calls that occur starting with a clock interrupt that awakens two sleeping processes, one of which has higher priority than the currently executing process.

We can create a process and give it a low priority and then make two processes with the same high priority. We can then sleep both of the high priority processes for the same amount of milliseconds. Having the same milliseconds will make them timeout at the same time causing wakeup() to deal with moving both processes from the sleeping queue to the ready queue. However, wakeup() is called from an interrupt dispatcher, therefore it can call ready without interrupts being disabled. Since this can happen, wakeup() will not call resched() but instead call resched_ctrl() which will cause a content switch to happen if there is a higher priority process ready. We know that a higher priority process is ready so we will end up context switching to the first in the ready queue.

#b. Does sleepms(3) guarantee a minimum delay of 3 milliseconds, an exact delay of 3 milliseconds, or a maximum delay of 3 milliseconds?

sleepms(3) will move a process to the sleep queue for a minimum of 3 milliseconds. The function relies entirely on what the clkinit() set a millisecond to be interrpreted as. Another factor is how long each interrupt will take to run because while sleepms(3) adds a process to the sleep queue we need to actually have interrupts like clkhander() call wakeup() to remove processes on the sleep queue that have a key of 0. Another factor that comes into the scene is what QUANTUM is set at so that infinitely running processes reschedule at some point. Lastly, the processes that gets put on the sleep queue may or may not have the highest priority. Therefore, even when the process wakes up it may not be the first to run.

With a lot of other processes running and interrupts being called, we cannot guarantee an exact delay or a maximum delay. There is a for sure chance there will be a longer delay then the sleep time given in sleepms.


## 2. Suppose a system contains three processes: a low-priority process, L, that is sleeping, and two high-priority processes, H1 and H2, that are eligible to execute. Further suppose that immediately after the scheduler switches to process H1, a clock interrupt occurs, process L becomes ready, and the interrupt handler calls resched. Although L will not run, resched will switch from H1 to H2 without giving H1 its quantum. Propose a modification to resched that ensures a process will not lose control of the processor unless a higher-priority process becomes ready or its time slice expires.

To get H1 its quantum the modifications to resched will have to check the value of preempt before choosing which process to resched to. In the question we have process L set to PR_SLEEP, and both H1 and H2 set to PR_READY. We will switch to the ready process H1 and give it the state of PR_CURR which will then begin to run. However, we will have a clock interrupt that will set process L to PR_READY where it will then be on the ready queue with process H2. However, the clock interrupt after placing L onto the ready list will call resched which will check if the H1 has a greater priority then the first key on the readylist (ptold->prprio > firstkey(readylist)).
At this moment we would have moved on to process H2 because H1 does not have a greater priority than H2. Both H1 and H2 have the exact same priority. The new implementation will check to see if the current running process has a greater priority than the key of the first process on the ready list like above. However, right after this, we will have another check to determine if the current running process has the same priority of the first process on the ready list and if preempt (our preemption variable) is greater than 0. Now if the current running process priority is the same as first process priority in the ready list, we will continue running the currently running process until preempt is 0. When preempt reaches 0 then resched will pass through and set H1 back on the ready list which will be a place after H2.

if (ptold->prio == firstkey(readylist) && preempt > 0) {
	return;
}



## 3. Assume that in the course of debugging you begin to suspect that a process is making incorrect calls to high-level I/O functions (e.g., calling seek on a device for which the operation makes no sense). How can you make a quick change to the code to intercept such errors and display the process ID of the offending process?

When a process is making incorrect calls to high-level I/O functions then the variable for device type must be set and checked. If we are able to check the device type then we can determine which I/O functions can be used on it. Depending on what I/O function was used, the current process id and the error type can be used to a log in an error file.  The extra code will be added to both the device initializer and the high level I/O functions.

When running seek we check to make sure that we do not had a bad device given the device descriptor. After checking for a good device, we check to make sure the device given should be able to invoke the given function.
