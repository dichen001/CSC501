/* vgetmem.c - vgetmem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 * vgetmem  --  allocate virtual heap storage, returning lowest WORD address
 *------------------------------------------------------------------------
 */
WORD	*vgetmem(nbytes)
	unsigned nbytes;
{

	STATWORD ps;    
	struct	mblock	*p, *q, *leftover;
	struct	mblock vmemlist;
	vmemlist = *(proctab[currpid].vmemlist);
	disable(ps);
	if (nbytes == 0 || nbytes > vmemlist.mlen || vmemlist.mnext== (struct mblock *) NULL) {
		kprintf("ERROR in getmem, nbytes required is %d, pages is backing store is %d\n",nbytes, vmemlist.mlen);
		restore(ps);
		return( (WORD *)SYSERR);
	}
	nbytes = (unsigned int) roundmb(nbytes);
	for (q= &vmemlist,p=vmemlist.mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext)
		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			kprintf("nbytes required is equal to the pages in backing store, which is %d\n", nbytes);
			restore(ps);
			return( (WORD *)p );
		} else if ( p->mlen > nbytes ) {
			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			kprintf("%d bytes got from backing store[%d], leftover is %d\n", nbytes, proctab[currpid].store, leftover->mlen);
			restore(ps);
			return( (WORD *)p );
		}
	restore(ps);
	return( (WORD *)SYSERR );

}


