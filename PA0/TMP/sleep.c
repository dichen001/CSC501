/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
SYSCALL	sleep(int n)
{
extern int flag;
if(flag){
	extern int freq[6][NPROC];
        freq[4][currpid]++;
	kprintf("########################################\n");
        kprintf("System Call(4) 'sleep' being called by \"%s\" (pid=%d)\n", proctab[currpid].pname, currpid);
	kprintf("########################################\n");
}

	STATWORD ps;    
	if (n<0 || clkruns==0)
		return(SYSERR);
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	return(OK);
}
