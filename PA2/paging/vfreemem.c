/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{	 
	struct	mblock * x = block;
	STATWORD ps;    
	disable(ps);
	kprintf("x=%8x \t x->mnext=%8x \t x->mlen=%d\n",
			x,x->mnext,x->mlen);
	kprintf("block=%8x \t block->mnext=%8x \t block->mlen=%d \t size=%d\nproctab[currpid].vhpno=%8x \t proctab[currpid].vhpnpages=%8x\n",
			block,block->mnext,block->mlen,size,proctab[currpid].vhpno*NBPG,proctab[currpid].vhpnpages*NBPG);
	//STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;
	struct	mblock vmemlist;
	vmemlist = *(proctab[currpid].vmemlist);
	// check if block address is within the the memory address range of the virtual heap. 
	if (size==0 || (unsigned)block > (unsigned)(proctab[currpid].vhpno + proctab[currpid].vhpnpages)*NBPG
	    || ((unsigned)block)<((unsigned) proctab[currpid].vhpno*NBPG))
		return(SYSERR);
	size = (unsigned)roundmb(size);
	kprintf("entered\n");
	//disable(ps);
	for( p=vmemlist.mnext,q= &vmemlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext ){
		 kprintf("block=%8x \t block->mnext=%8x \t block->mlen=%d\nq=%8x \t q->mnext=%8x \t q->mlen=%d \np=%8x \t p->mnext=%8x \t p->mlen=%d \n",
			block,block->mnext,block->mlen,
			q,q->mnext,q->mlen,p,
			p->mnext,p->mlen);
	}
	// check if the memeory going to be free is reasonable, i.e within the range.
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &vmemlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		return(SYSERR);
	}
	// if the block address is right at the top of free memory, we just add this block behind the free memory block.
	if ( q!= &vmemlist && top == (unsigned)block ){
		kprintf("if the block address is right at the top of free memory, we just add this block behind the free memory block.\n");
		kprintf("block=%8x \t block->mnext=%8x \t block->mlen=%d\nq=%8x \t q->mnext=%8x \t q->mlen=%d \np=%8x \t p->mnext=%8x \t p->mlen=%d \n",
			block,block->mnext,block->mlen,
			q,q->mnext,q->mlen,p,
			p->mnext,p->mlen);
		q->mlen += size;
	}
	else {
	// else we add this bolck before the free memory block.
	 kprintf("else we add this bolck before the free memory block.\n");
	kprintf("block=%8x \t block->mnext=%8x \t block->mlen=%d\nq=%8x \t q->mnext=%8x \t q->mlen=%d \np=%8x \t p->mnext=%8x \t p->mlen=%d \n",
			block,block->mnext,block->mlen,
			q,q->mnext,q->mlen,p,
			p->mnext,p->mlen);
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	// add-up as one
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		kprintf("add-up as one \n");
		q->mlen += p->mlen;
		q->mnext = p->mnext;
		kprintf("block=%8x \t block->mnext=%8x \t block->mlen=%d\nq=%8x \t q->mnext=%8x \t q->mlen=%d \np=%8x \t p->mnext=%8x \t p->mlen=%d \n",
			block,block->mnext,block->mlen,
			q,q->mnext,q->mlen,p,
			p->mnext,p->mlen);
	}
	restore(ps);
	return(OK);
}
