/* releaseall.c - Release the specified lockes */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
void deque(int pid, int ldes);

SYSCALL releaseall(nargs,args)
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	STATWORD 	ps;    
	unsigned long	*a;		/* points to list of args	*/
	int ldes;
	disable(ps);

	if(GDB)
		kprintf("RRRRR: proc[%d]:%s \n",currpid, proctab[currpid].pname);

	a = (unsigned long *)(&args) + (nargs-1); /* last argument	*/
	for(nargs; nargs > 0 ; nargs--)
	{
		if(GDB)
			kprintf("*a = %d\n",*a);
		ldes = *a--;
		if(GDB)
			kprintf("releasing lock[%d] status=%d type=%d prio=%d)\n", ldes, locktab[ldes].lstatus, locktab[ldes].lstate, locktab[ldes].lprio);
		//for the global lock table, they should only concern about the lcok status.
		if (locktab[ldes].lstatus == L_USED)
		{
			deque(currpid, ldes);
			proctab[currpid].locks[ldes].lstatus = L_FREE;
			proctab[currpid].locks[ldes].next =-1;
			proctab[currpid].locks[ldes].lstate =-1;
			proctab[currpid].locks[ldes].lprio =-1;	
			//ready(pid,RESCHNO);	
		}
		else
			if(GDB)
				kprintf("ERROR: cannot release a free lock.\n");
		//do something.	
	}

	if(GDB)
		kprintf("RRRRR: proc[%d]:%s release done! going to resched\n",currpid, proctab[currpid].pname);
	resched();
	restore(ps);
	return(OK);
}

void deque(int pid, int ldes){
	int pre, curr;
	curr = locktab[ldes].head;
	if(GDB)
		kprintf("in deque, head = proc[%d]\n",curr);
	// removing the head.
	if(curr == pid )
	{
		// removing the only process having this lock
		if(proctab[curr].locks[ldes].next == -1)
		{
			locktab[ldes].head = -1;
			locktab[ldes].lstatus = L_FREE;
			locktab[ldes].lstate = -1;
			locktab[ldes].lprio = -1;
			if(GDB)
				kprintf("deque proc[%d]:%s.  No process waiting for lock[%d]. it's free now\n",pid, proctab[pid].pname,ldes);
		}
		else
		{
			locktab[ldes].head = proctab[curr].locks[ldes].next;
			if(GDB)
				kprintf("deque proc[%d]:%s.  proc[%d] is the NEW head in waiting Q of lock[%d].\n",pid, proctab[pid].pname, locktab[ldes].head, ldes);
			int hpid = locktab[ldes].head;	// hpid ==>> process id with highest priority in waiting list.
			locktab[ldes].lstate =  proctab[hpid].locks[ldes].lstate;
			locktab[ldes].lprio = proctab[hpid].locks[ldes].lprio;
			if(GDB)
				kprintf("now proc[%d]:%s(pstate= %d) get lock[%d]:{type=%d prio=%d} )\n",hpid, proctab[hpid].pname, proctab[hpid].pstate, ldes,locktab[ldes].lstate, locktab[ldes].lprio);
			if(proctab[hpid].pstate == PRWAIT)
			{
				if(GDB)
					kprintf("going to put pro[%d] into the ready Queue\n",hpid);
				ready(hpid, RESCHNO);
			}
			
		}
		return;
	}
	// removing from somewhere in the middle of the waiting Queue.
	pre = curr;
	curr = proctab[curr].locks[ldes].next;
	while(curr != -1){
		if(curr == pid)
		{
			proctab[pre].locks[ldes].next = proctab[curr].locks[ldes].next;
			if(GDB)
				kprintf("proc[%d]:%s is removed from the queue.\n",pid, proctab[pid].pname);
			int hpid = locktab[ldes].head;	// hpid ==>> process id with highest priority in waiting list.
			locktab[ldes].lstate =  proctab[hpid].locks[ldes].lstate;
			locktab[ldes].lprio = proctab[hpid].locks[ldes].lprio;
			if(GDB)
				kprintf("now proc[%d]:%s(pstate= %d) get lock[%d]:{type=%d prio=%d} )\n",hpid, proctab[hpid].pname, proctab[hpid].pstate, ldes,locktab[ldes].lstate, locktab[ldes].lprio);
			if(proctab[hpid].pstate == PRWAIT)
			{
				if(GDB)
					kprintf("going to put pro[%d] into the ready Queue\n",hpid);
				ready(hpid, RESCHNO);
			}
			return;
		}
		if(GDB)
			kprintf("in deque while, curr=%d pid=%d",curr,pid);
		pre = curr;
		curr = proctab[curr].locks[ldes].next;
		if(GDB)
			kprintf(" nxet=%d\n",curr);
	}
	if(GDB)
		kprintf("ERROR in deque: cann't found the lock!\n");
}