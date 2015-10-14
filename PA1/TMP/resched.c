/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
//-------- for PA1 -------------------------------
 extern int sched_class;
int epoch;
int epoch_id;
int new_epoch_flag = 0;
//-----------------------------------------------
LOCAL star_new_epoch(int epoch_id);
LOCAL switch_process(register struct	pentry	*optr);


int resched()
{	
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	register struct	pentry	*ptr;	/* pointer to temp process entry */

	if(sched_class == 0){
		//kprintf("origina1 scheduler current running %d\n",currpid);
		/* no switch needed if current process priority higher than next*/
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		   (lastkey(rdytail)<optr->pprio)) {
		 	//kprintf("no switch : [%s]",optr->pname);
			return(OK);
		}
		
		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
		//kprintf("before (preempt = QUANTUM) %s, preempt= %d\n",nptr->pname,preempt);
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
		
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		/* The OLD process returns here when resumed. */
		return OK;

	}

	else if(sched_class == LINUXSCHED ){
		//exchange the order of epoch and preempt later
		//UPDATE 
		if(preempt <= 0){
			preempt =QUANTUM;
			//kprintf("### LINUXSCHED scheduler current running %d\n",currpid);
			if(currpid == 0 && nonempty(rdyhead))
				if( lastkey(rdytail) <= 0){
					star_new_epoch(epoch_id);
					return OK;
				}
				else{
					switch_process( &proctab[currpid] );
					return OK;
				}
					


			ptr= &proctab[currpid];
			//kprintf("before update %s , preempt= %d epoch= %d counter= %d goodness= %d\n", ptr->pname,preempt,epoch,ptr->counter,ptr->goodness);
			
			//update
			if(epoch_id != 0 && new_epoch_flag != 1){
				epoch -= QUANTUM;
				ptr->counter -= QUANTUM;
				if (ptr->counter <= 0)
					ptr->goodness = 0;
				else
					ptr->goodness = ptr->counter + ptr->pprio;
			}
			if (new_epoch_flag ==1)
				new_epoch_flag = 0;
			
			//kprintf("after update %s , preempt= %d epoch= %d counter= %d goodness= %d\n", ptr->pname,preempt,epoch,ptr->counter,ptr->goodness);						
		}

		//kprintf("### LINUXSCHED scheduler current running %d preempt is %d\n",currpid,preempt);

		//first epoch set goodness of main.
		if(epoch_id == 0){
			epoch = proctab[currpid].goodness = proctab[currpid].counter = proctab[currpid].quantum = proctab[currpid].pprio;
			epoch_id++;
		}

		/* new epoch begins, reset all the goodness and quantum in ready Q.*/
		if(epoch <= 0){
			star_new_epoch(epoch_id);
			return OK;
		}
		
		//kprintf("current counter=%d goodness=%d  lastkey(rdytail)=%d\n",proctab[currpid].counter,proctab[currpid].goodness,lastkey(rdytail));
		
		//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& ke yi shan le %%%%%%%%%%%%%
		/*no runable process in this Ready Q, start a new epoch*/
		if ( currpid == 0  && isempty(rdyhead) ){
			//kprintf("no runable process in Ready Q, current process is %s\n", proctab[currpid].pname);
			return OK;
		}

		/*no need to resched*/
		if (( (optr= &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail) < optr->goodness) && (optr->counter > 0) ){
		   //	kprintf("NO RESCHED: currpid %d preempt= %d epoch= %d counter= %d goodness= %d\n", currpid,preempt,epoch,optr->counter,optr->goodness);
			return OK;
		}

		preempt = QUANTUM;
		switch_process(optr);

		return OK;

	}
	
}

LOCAL star_new_epoch(int epoch_id){
	if(proctab[currpid].counter <= 0  && lastkey(rdytail) <= 0 && currpid != 0){
		switch_process( (&proctab[currpid]) ) ;
		return OK;
	}
	epoch_id++;

	//kprintf("\n****************\nNO.%d  epoch \n", epoch_id);
	STATWORD ps;  
	disable(ps);

	epoch = 0;
	//get all the ready processes in ready queue for later re-arranging
	int ready_queue[NPROC],id=0;
	int next = q[rdyhead].qnext;
	while (next < NPROC && proctab[next].pstate == PRREADY){
		//kprintf("dequeue (%d) id =%d\n",next,id);		
		ready_queue[id++] = next;
		next = q[next].qnext;
		getfirst(rdyhead);		
	}
	while (id--){
		next = ready_queue[id];
		//kprintf("enqueue (%d) id =%d\n",next,id);	

		//check priority change
		if(proctab[next].newprio != 0 && proctab[next].newprio != proctab[next].pprio)
			proctab[next].pprio = proctab[next].newprio;

		if (proctab[next].counter < 0)
			proctab[next].counter = 0; 
		
		proctab[next].quantum = floor(proctab[next].counter/2) + proctab[next].pprio;
		proctab[next].counter = proctab[next].quantum;
		proctab[next].goodness = proctab[next].counter + proctab[next].pprio;
		epoch += proctab[next].quantum;
		insert(next,rdyhead,proctab[next].goodness);
		//kprintf("after enqueuing [%s], preempt= %d, epoch= %d, prio= %d, counter= %d, goodness=%d\n",proctab[next].pname,preempt,epoch,proctab[next].pprio,proctab[next].counter,proctab[next].goodness);
	}

	new_epoch_flag = 1;
	restore(ps);
}


LOCAL switch_process(register struct	pentry	*optr){
	/* move old process to ready Q */		
	register struct	pentry	*nptr;	/* pointer to new process entry */
	//kprintf("$$ RESCHED NEEDED $$\n");
	//kprintf("move current running [%s] to Ready Q \n",optr->pname);
	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->goodness);
	}
	
	/* higher goodness in ready Q or quantem runs out */
	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/	
	//kprintf("*******move %s from Ready Q to run************\n", nptr->pname);

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	/* The OLD process returns here when resumed. */
}
