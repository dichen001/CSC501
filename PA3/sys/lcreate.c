/* lcreate.c - create a lock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate()
{
	STATWORD ps;    
	disable(ps);

	int	i,lock;
	for (i=0 ; i<NLOCKS ; i++) {
		lock=nextlock--;
		if (nextlock < 0)
			nextlock = NLOCKS-1;
		if (locktab[lock].lstatus == L_FREE) {
			locktab[lock].lstatus = L_USED;
			if(GDB)
				kprintf("lock[%d] is allocated to proc[%d]:%s\n",
				lock, currpid, proctab[currpid].pname);

			restore(ps);
			return(lock);
		}
	}

	/* @@@@@  lqhead and lqtail need 2nd thought @@@@ */
	restore(ps);
	return(SYSERR);
}
