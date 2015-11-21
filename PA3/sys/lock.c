/* lock.c - Acquisition of a lock for read/write */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>


/**
 * In your implementation, no readers should be kept waiting unless 
 * (i) a writer has already obtained the lock, or 
 * (ii) there is a higher priority writer already waiting for the lock. 
 * 
 * ----------------- below is for releaseall.c -------------------------
 * Hence, when a writer or the last reader releases a lock, the lock 
 * should be next given to a process having the highest waiting priority 
 * for the lock. In the case of equal waiting priorities, the lock will 
 * be given to the process that has the longest waiting time 
 * (in milliseconds) on the lock. 
 * 
 * If the waiting priorities are equal and the waiting time difference 
 * is within 1 second, writers should be given preference to acquire 
 * the lock over readers. 
 * 
 * In any case, if a reader is chosen to have a lock, then 
 * all the other waiting readers having priority not less than 
 * that of the highest-priority waiting writer for the same lock 
 * should also be admitted.
 */


/**
Control is returned only when the process is able to acquire the lock. 
Otherwise, the calling process is blocked until the lock can be obtained.

Acquiring a lock has the following meaning:
1. The lock is free, i.e., no process is owning it. 
	In this case the process that requested the lock gets the lock 
	and sets the type of locking as READ or WRITE.

2. Lock is already acquired:
	a. For READ:
		If the requesting process has specified the lock type as READ 
		and has sufficiently high priority (not less than the highest 
		priority writer process waiting for the lock), it acquires 
		the lock, else not.
	b. For WRITE:
		In this case, the requesting process does not get the lock 
		as WRITE locks are exclusive.
 */

LOCAL insert_lock(int ldes, int prio);
int highest_write_prio(int ldes);

int lock (int ldes, int type, int priority){
	STATWORD ps; 
	disable(ps);
	// we should also store information in the proctab as the local information
	// while keep the locktab[] monitor the status of all the locks.
	proctab[currpid].locks[ldes].lstatus = L_USED;
	proctab[currpid].locks[ldes].lstate =type;
	proctab[currpid].locks[ldes].lprio = priority;
	/**
	*	Acquiring a lock has the following meaning:
	*	1. The lock is free, i.e., no process is owning it. 
	*	In this case the process that requested the lock gets the lock 
	*	and sets the type of locking as READ or WRITE.
	**/
	if(GDB)
		kprintf("LLLL: proc[%d]:%s requesting for lock[%d] type=%d prio=%d)\n",currpid,proctab[currpid].pname,ldes,type,priority);
	//for the global lock table, they should only concern about the lcok status.
	if (locktab[ldes].lstatus == L_USED && locktab[ldes].lstate == -1)
	{
		locktab[ldes].lstate = type;
		locktab[ldes].lprio = priority;
		if(GDB)
			kprintf("lock[%d] state: FREE to USED, type is %d, piro=%d\n",ldes,type,priority);
		insert_lock(ldes, priority);
		restore(ps);
		return OK;
	}

		
	/**
	*	2. Lock is already acquired:
			a. For READ:
				If the requesting process has specified the lock type as READ 
				and has sufficiently high priority (not less than the highest 
				priority writer process waiting for the lock), it acquires 
				the lock, else not.
			b. For WRITE:
				In this case, the requesting process does not get the lock 
				as WRITE locks are exclusive.
	**/
	if (locktab[ldes].lstate == WRITE)
	{
		if(GDB)
			kprintf("lock[%d] state: WRITE, going to insert and resched\n",ldes);
		proctab[currpid].pwaitret = OK;
		proctab[currpid].pstate = PRWAIT;
		insert_lock(ldes, priority);
		resched();
		// restore(ps);
		// return OK;
	}
	
	if ( locktab[ldes].lstate == READ )
	{
		if(GDB)
			kprintf("lock[%d] state: READ \nrequesting process's read_prio = %d, highest_write_prio=%d\n",ldes,priority,highest_write_prio(ldes));
		insert_lock(ldes, priority);
		if(type == READ && priority >= highest_write_prio(ldes))
		{
			if(GDB)
				kprintf("proc[%d]:%s can READ\n",currpid,proctab[currpid].pname);
			int curr; 
			curr = locktab[ldes].head;
			while(curr != -1){
				if(GDB)
					kprintf("traversing lock_Q: pid=%d\n",curr);
				curr = proctab[curr].locks[ldes].next;
			}
			restore(ps);
			return OK;
		}
		if(GDB)
			kprintf("Cannot acquire the lock, going to resched\n");
		proctab[currpid].pwaitret = OK;
		proctab[currpid].pstate = PRWAIT;
		resched();
		// restore(ps);
		// return OK;
	}

	/**
	 * You must implement your lock system such that waiting on a lock will 
	 * return a new constant DELETED instead of OK when returning due to a 
	 * deleted lock. This will indicate to the user that the lock was deleted 
	 * and not unlocked. As before, any calls to lock() after the lock is 
	 * deleted should return SYSERR.
	 */
	if (proctab[currpid].locks[ldes].lstate == DELETED)
	{
		if(GDB)
			kprintf("ERROR: DELETED\n");
		restore(ps);
		return DELETED;
	}
	restore(ps);
	return OK;
}

int highest_write_prio(int ldes){
	int curr,pre; 
	curr = locktab[ldes].head;
	if(curr == -1)
		return -1;
	while(curr != -1)	{
		if(proctab[curr].locks[ldes].lstate == WRITE)
			return proctab[curr].locks[ldes].lprio;
		pre = curr;
		curr = proctab[curr].locks[ldes].next;
		//kprintf("pre:proc[%d].prio=%d, curr: %d\n",pre->pid, proctab[pre->pid].locks[ldes].lprio, curr==NULL?-1:curr->pid);
	}
	return -1;
}


/* should be indexed through their pid. */
LOCAL insert_lock(int ldes, int insert_prio){
	int pre, curr;
	curr = locktab[ldes].head;
	if(GDB)
		kprintf("IIIIIIIIIIIIII   head=%d\n",curr);
	if(curr == -1){
		//curr = (struct lqueue *) getmem(sizeof(struct lqueue *));
		locktab[ldes].head = currpid;
		locktab[ldes].next = -1;
		proctab[currpid].locks[ldes].head = currpid;
		proctab[currpid].locks[ldes].next = -1;
		if(GDB)
			kprintf("head = %d nxet=%d\n",locktab[ldes].head, locktab[ldes].next);
		if(GDB)
			kprintf("proc[%d]:%s enter the waiting Q of locktab[%d], its piro=%d\n",currpid,proctab[currpid].pname,ldes,insert_prio);
		return;
	}
	int curr_prio = proctab[curr].locks[ldes].lprio;
	if(insert_prio > curr_prio)	{
		proctab[currpid].locks[ldes].head = currpid;
		proctab[currpid].locks[ldes].next = locktab[ldes].head;
		if(GDB)
			kprintf("proc[%d]:%s .prio=%d > head_of_waiting_Q( proc[%d] )\n",currpid,proctab[currpid].pname,insert_prio,locktab[ldes].head);
		locktab[ldes].head = currpid;
		return;
	}
	while(curr != -1 && insert_prio <= curr_prio)	{
		pre = curr;
		curr = proctab[curr].locks[ldes].next;
		if(curr != -1)
			curr_prio = proctab[curr].locks[ldes].lprio;
		if(GDB)
			kprintf("pre:proc[%d].prio=%d, curr: %d\n",pre, proctab[pre].locks[ldes].lprio, curr);
	}
	proctab[pre].locks[ldes].next = currpid;
	proctab[currpid].locks[ldes].next = curr;
	if(GDB)
		kprintf("insert proc[%d] between proc[%d] and proc[%d]\n",currpid,pre,curr);
	return;

}