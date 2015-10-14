/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
SYSCALL signal(int sem)
{
extern int flag;
if(flag){
	extern int freq[6][NPROC];
        freq[3][currpid]++;
	kprintf("########################################\n");
        kprintf("System Call(3) 'signal' being called by \"%s\" (pid=%d)\n", proctab[currpid].pname, currpid);
	kprintf("########################################\n");
}

	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	return(OK);
}
