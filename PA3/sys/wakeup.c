/* wakeup.c - wakeup */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>

/*------------------------------------------------------------------------
 * wakeup  --  called by clock interrupt dispatcher to awaken processes
 *------------------------------------------------------------------------
 */
INTPROC	wakeup()
{
       	int proc;
        while (nonempty(clockq) && firstkey(clockq) <= 0)
        {
        	proc = q[clockq].qnext;
        	ready(getfirst(clockq),RESCHNO);
        }     
        //kprintf("\n\nwaking up proc[%d]: %s\n\n",proc, proctab[proc].pname);   
	if ((slnempty = nonempty(clockq)))
		sltop = & q[q[clockq].qnext].qkey;
	resched();
        return(OK);
}
