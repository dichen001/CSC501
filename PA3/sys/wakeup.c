/* wakeup.c - wakeup */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <lock.h>
/*------------------------------------------------------------------------
 * wakeup  --  called by clock interrupt dispatcher to awaken processes
 *------------------------------------------------------------------------
 */
int is_in_lock_Q_but_not_head(int pid){
	int i;
	for(i=0; i<NLOCKS; i++)
	{
		if(proctab[pid].locks[i].lstatus == L_USED && locktab[i].head != pid)
			{
				kprintf("\nproc[%d] is in lock[%d]'s Q, but it's not the head, head is proc[%d]\n\n",pid,i,locktab[i].head);
				return 1;
			}
	}	
	return 0;

}

INTPROC	wakeup()
{
       	int proc;
        while (nonempty(clockq) && firstkey(clockq) <= 0)
        {
        	proc = getfirst(clockq);
        	if (is_in_lock_Q_but_not_head(proc) == 1 )
        	{
        		proctab[proc].pwaitret = OK;
				proctab[proc].pstate = PRWAIT;
        	}
        	else
        		ready(proc,RESCHNO);
        }     
        //kprintf("\n\nwaking up proc[%d]: %s\n\n",proc, proctab[proc].pname);   
	if ((slnempty = nonempty(clockq)))
		sltop = & q[q[clockq].qnext].qkey;
	resched();
        return(OK);
}



