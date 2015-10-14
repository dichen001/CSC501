/* insert.c  -  insert */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>

/*------------------------------------------------------------------------
 * insert.c  --  insert an process into a q list in key order
 *------------------------------------------------------------------------
 */
int insert(int proc, int head, int key)
{
	int	next;			/* runs through list		*/
	int	prev;
//
	next = q[head].qnext;
	while (q[next].qkey < key)	/* tail has maxint as key	*/
		next = q[next].qnext;
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	q[proc].qkey  = key;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	
	/*TEST USE: implemet a loop to show the queue.*/
	/*
	next = q[head].qnext;
	int temp;
	temp = preempt;
	preempt = QUANTUM; 
	kprintf("@_@ after queue inserting of proc(%d) CURRPID: %d preempt: %d\n",proc,currpid,preempt);
	while (q[next].qnext != EMPTY){
		if (preempt <= 0)
			preempt = QUANTUM;
		kprintf("index= %d \t key= %d \t prev= %d \t next= %d CURRPID: %d preempt: %d\n",
			next,q[next].qkey,q[next].qprev,q[next].qnext,currpid,preempt);
		next = q[next].qnext;
	}
	preempt = temp;
	*/
	return(OK);
}
