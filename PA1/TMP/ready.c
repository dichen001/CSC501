/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
 extern int sched_class;
int ready(int pid, int resch)
{	
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	//************ for PAO_1 ***************
	if (!sched_class)
		insert(pid,rdyhead,pptr->pprio);
	else if(sched_class == LINUXSCHED )
		insert(pid,rdyhead,pptr->goodness);
	//**************************************
	if (resch)
		resched();
	return(OK);
}
