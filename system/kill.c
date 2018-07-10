/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	int32	i;			/* Index into descriptors	*/

	/* Disable interrupts and save mask */
	mask = disable();
	
	/* Check for bad pid or if pid is of the null process */
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	/* Check id the last user process has been killed  */
	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}


	/* Send the pid of the killed process to the parent process */
	send(prptr->prparent, pid);

	/*   */
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}

	/* Check to see if the process to be killed is the current running process */
	if (prptr->prstate == PR_CURR) {
	  /* Process to be killed is curent running process
	     Set state to DYING and resched */
	  prptr->prstate = PR_DYING;
	  resched();
	  restore(mask);
	  return OK;
	}


	freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}

	restore(mask);
	return OK;
}
