/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	/* no switch needed if current process priority higher than next*/
	preempt = QUANTUM;
	if(0)
		kprintf("current process[%d]: %s, lastkey()=%d, priority=%d\n", currpid,proctab[currpid].pname, lastkey(rdytail), proctab[currpid].pprio);
	if(0)
		kprintf("process state is %d, PRCURR=%d\n",proctab[currpid].pstate,PRCURR);
	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	
	/* force context switch */
	int proc;
	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;											
		if ((proc=q[rdytail].qprev) < NPROC)
			if(0)
				kprintf("before inserting, %s is in the tail of ready Q\n",proctab[proc].pname);
		if(0)
			kprintf("going to insert (%s) into ready Q\n",proctab[currpid].pname);
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	if ((proc=q[rdytail].qprev) < NPROC)
			if(0)
				kprintf("now, the tail of ready Q is %s\n",proctab[proc].pname);
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
	if(0)
		kprintf("new process is %s\n",nptr->pname);
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	if(0)
		kprintf("going to resched from %s to [%d]:%s\n",optr->pname,currpid,nptr->pname);
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
