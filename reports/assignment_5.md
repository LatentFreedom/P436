Nick Palumbo
Assignment 5 report

1. What is starvation (in reference to concurrency and semaphores)?

Starvation is when a process is eligible to run and use the processor but does not ever get a chance to. This can happen when a process has a very low priority and there are multiple higher priority processes that are going in a round robin. The low priority process will not get a chance to run unless a check is in place to make sure it gets to run at some time in the future. However, if the process never gets to run, yet it is eligible to "at some point", then it is considered to be starved of processor time. 


a. What does it mean for a thread/process to be starved?

A thread or process is considered to be starved if it does not get time to run.


b. Under what situations can it occur?

A situation when a process or thread can be starved is when it has a low priority and there are other high priority processes running in a round robin. A round robin implementation will loop around the high priority processes. Since the low priority process does not get to run since it is out of the round robin or because it simply has such a low priority it is considered to be starved from processor time. 

c. How do you convince someone that it can't occur in code that you have written?

In order to convince someone that starvation is not a problem, an implementation must be made to account for lower priority processes. There must be an again implementation where the high priority processes either lower their priority after each round until each and every process has ran. Then the priority can return to normal. Another implementation could be that the lower priority processes gain a priority level each round they are not ran. This implementation is the opposite of the first but will have similar effects. After the low priority process is ran because of its increase in priority the priority will return to its normal value where the process of againg will be done all over again. 

2. What is deadlock?

Deadlock happens when there are multiple processes trying to access a certian memory location but are stopped by a semaphore, mutex or other instance where a lock has been set but will not be released. If there is a lock set then processes will both be waiting on someone to unlock the semaphore or mutex but none will because they are all waiting on another to unlock it.

a. What does it mean for threads/processes to be deadlocked?

A thread or process is considered to be deadlocked when they are both waiting on an action that will never happen. If a lock is set then the lock must be unlocked before the threads or processes can continue but if no thread or process will ever unlock the lock then nobody will continue which leads to a deadlock.

b. Under what situations can it occur?

If multiple processes need to access a memory location but a lock has been set then there must be a process that unlocks the lock so the processes can continue. However, lets start the lock in a locked state. If we do not designate a process to first unlock the lock then nobody will be able to continue and all of the processes that need to take action aftert the lock is unlocked will be stalled and stay in a waiting state forever.

c. How do you convince someone that it can't occur in code that you have written?

To convince someone that a deadlock will never happen an implementation must be implemented to account for some base cases. If a resource is needed but only a certain amount of processes can use it at a time then mutual exclusion must be implemented to allow the resource to be used by the specified amount of processes. Another implementation is a hold and wait. If a resource is being held by another process then the other processes must wait for the process to be released in order for another process to take action. However, these implementations must designate how a resource is locked and unlocked. Like stated above, if no process can unlock a resource then there will be a deadlock. 

Another interesting idea is implementing a way to detect how long processes are waiting and somehow determine if the process should be killed off, restarted, or if indeed there is a deadlock and a lock needs to be reset or unlocked. 