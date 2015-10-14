/* newqueue.c  -  newqueue */

#include <conf.h>
#include <kernel.h>
#include <q.h>

/*------------------------------------------------------------------------
 * newqueue  --  initialize a new list in the q structure
 *------------------------------------------------------------------------
 */
int newqueue()
{
	struct	qent	*hptr;
	struct	qent	*tptr;
	int	hindex, tindex;

	hptr = &q[ hindex=nextqueue++]; /* assign and rememeber queue	*/
	tptr = &q[ tindex=nextqueue++]; /* index values for head&tail	*/
	hptr->qnext = tindex;
	hptr->qprev = EMPTY;
	hptr->qkey  = MININT;
	tptr->qnext = EMPTY;
	tptr->qprev = hindex;
	tptr->qkey  = MAXINT;
	
	//kprintf("in newqueue, hptr= %d  .key= %d .prev= %d .next= %d\n",hindex,q[hindex].qkey, q[hindex].qprev,q[hindex].qnext);
	//kprintf("in_newqueue, tptr= %d  .key= %d .prev= %d .next= %d\n",tindex,q[tindex].qkey, q[tindex].qprev,q[tindex].qnext);
	//************
	//STATWORD ps;    
	//disable(ps);
	/*
	struct	qent	*real_hptr;
	struct	qent	*real_tptr;
	int	real_hindex, real_tindex;
	real_hptr = &q[ real_hindex=nextqueue/2-1]; 
	real_tptr = &q[ real_tindex=nextqueue/2]; 
	real_hptr->qnext = real_tindex;
	real_hptr->qprev = EMPTY;
	real_hptr->qkey  = MININT;
	real_tptr->qnext = EMPTY;
	real_tptr->qprev = real_hindex;
	real_tptr->qkey  = MAXINT;
	//*************
	
	kprintf("in newqueue, real_hptr= %d  .key= %d .prev= %d .next= %d\n",real_hindex,q[real_hindex].qkey, q[real_hindex].qprev,q[real_hindex].qnext);
	kprintf("in_newqueue, real_tptr= %d  .key= %d .prev= %d .next= %d\n",real_tindex,q[real_tindex].qkey, q[real_tindex].qprev,q[real_tindex].qnext);
	*/
	//restore(ps);
	
	return(hindex);
}
