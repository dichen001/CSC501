/* linit.c - initialize locks */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

void linit(){
	int i;
	for (i=0 ; i<NLOCKS ; i++) {	/* initialize semaphores */
		locktab[i].lstatus = L_FREE;	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ need 2nd thought @@@@@@@@@@@
		locktab[i].lstate = -1;
		locktab[i].lprio = -1;
		//@@@@ the queue still need to be implemented @@@@
		locktab[i].head = -1;
		locktab[i].next = -1;
	}	
	kprintf("lock initialized\n");
}

