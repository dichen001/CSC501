/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);

	//*******for PA2
	int i;
	for(i=0; i<16; i++)
	{
		if(bsm_tab[i].bs_status == BSM_MAPPED && bsm_tab[i].bs_pid == pid && bsm_tab[i].mapping_num > 0)
		{
			//kprintf("1 KILL: bs[%d].mapping=%d  proc=%s-(%d), vpno=%d, npages=%d\n", i, bsm_tab[i].mapping_num, proctab[pid].pname, pid, bsm_tab[i].bs_vpno, bsm_tab[i].bs_npages);
			free_bsm(i);
			//xmunmap(proctab[pid].bsmap[i].bs_vpno);
			//kprintf("2 KILL: bs[%d].mapping=%d  proc=%s-(%d), vpno=%d, npages=%d\n", i, bsm_tab[i].mapping_num, proctab[pid].pname, pid, bsm_tab[i].bs_vpno, bsm_tab[i].bs_npages);
		}
	}
	//*******for PA2
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
