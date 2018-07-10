Nick Palumbo
Assignment 4 report

1. Consider a modification to the code that stores processor state in the process table instead of on the process’s stack (i.e., assume the process table entry contains an array that holds the contents of registers). What are the advantages of each approach?


Modifying the code that stores processor state can have advantages. Storing the processor state in the process table instead of the process stack will have it's variables put into the process table which is already allocated in memory. There will be no need to allocate more memory on the stack with each context switch for the registers. Also, if we are saving values into the process table this would not be done in assembly. 

However, the difference is a memory location trade off. We will not be allocating memory on the stack but in the heap. 

The code needed for the tradeoff will change the way the process table structure is implemented. There will need to be a new variable for process state variables. The variables can be given their own variable or a new structure can be created for the variables. We can make a structure to mimic that of the registers and store that structure in a new variable which we can call registers in the process table. 


// I am not sure what kind of pseudocode is desired //


2. When a process kills itself, kill deallocates the stack and then calls resched, which means the process continues to use the deallocated stack. Redesign the system so a current process does not deallocate its own stack, but instead moves to a new state, PR_DYING. Arrange for whatever process searches the process table to look for dying processes, free the stack, and move the entry to PR_FREE.

First thing to do is modify the process.h to include the new state PR_DYING. I made the new state the value 8 because it is the next value after the previous states. After creating the new state I changed the code within kill to check if the process to be killed was the current running process.

To do this I checked to see if the process state was equal to PR_CURR. If the state of the process is current then the state will be changed to PR_DYING, reshed will be called, the interrupts will be enabled with the mask, and then a return of OK will happen.

After implementing the portion of kill I still needed to remove the memory of the stack. This removal of the stack memory is therefore within resched. When resched is called processes within the process table will be traversed to find a process to run next. However, a new check had to be implemented. I added an "else if" when checking the process state of the old process was the value PR_DYING. If we check the state and it is indeed PR_DYING then the process for the PR_DYING process will have it's stack freed from memory and the process will then be given the state PR_FREE so it can be used in the future.

3. Function resume saves the resumed process’s priority in a local variable before calling ready. Show that if it references prptr->prprio after the call to ready, resume can return a priority value that the resumed process never had (not even after resumption).

resume saves a local variable of the process's priority because it may change. Right after the local process priority is saved ready is called. The function ready will add the process to the ready list where it will be eligible to run. The point where the priority can become different is in the function call to resched within ready.

ready will call reshed and that will make it so the process with the highest priority is switched to. Now the process priority will be different if the current running process does not get resched defered and continues to run.

The path taken is resume->ready->resched. After resched we go back to ready where OK is returned back to resume. In resume the interupt mask is restored and the process is resumed.

In order to see the need for the local priority I have implemented a print statement within resume to attempt to catch when the priority difference happens. I have also implemented a shell command resume_debug that will create and resume processes that will simply sleep for a few seconds.