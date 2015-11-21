/* releaseall.c - Release the specified lockes */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

SYSCALL ldelete(int ldes){
	STATWORD 	ps;    
	int curr, pre;
	disable(ps);

	if(GDB)
		kprintf("DDDDD: proc[%d]:%s \n",currpid, proctab[currpid].pname);

	if(GDB)
		kprintf("deleting lock[%d] status=%d type=%d prio=%d)\n", ldes, locktab[ldes].lstatus, locktab[ldes].lstate, locktab[ldes].lprio);
	
	curr = locktab[ldes].head;
	while(curr != -1)
	{
		pre = curr;
		curr = proctab[curr].locks[ldes].next;
		proctab[pre].locks[ldes].lstatus = L_FREE;
		proctab[pre].locks[ldes].lstate = DELETED;
		proctab[pre].locks[ldes].lprio = -1;	
		proctab[pre].locks[ldes].next = -1;
		if(GDB)
			kprintf("lock[%d] deleted for proc[%d]:%s, next is proc[%d]\n",ldes,pre,proctab[pre].pname,curr);
		if(proctab[pre].pstate == PRWAIT)
		{
			if(GDB)
				kprintf("going to put pro[%d] into the ready Queue\n",pre);
			ready(pre, RESCHNO);
		}
	}


	//for the global lock table, they should only concern about the lcok status.
	if (locktab[ldes].lstatus == L_USED)
	{
		locktab[ldes].head = -1;
		locktab[ldes].lstatus = L_FREE;
		locktab[ldes].lstate = -1;
		locktab[ldes].lprio = -1;
		//ready(pid,RESCHNO);	
	}
	else
		if(GDB)
			kprintf("delete a free lock.(no need to do anything)\n head=%d,status=%d,state=%d,prio=%d\n",locktab[ldes].head, locktab[ldes].lstatus, locktab[ldes].lstate, locktab[ldes].lprio);
	//do something.	

	if(GDB)
		kprintf("DDDDD: delete done! going to resched\n",currpid, proctab[currpid].pname);
	resched();
	restore(ps);
	return(OK);
}
