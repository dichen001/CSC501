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
	disable(ps);
	struct	mblock	*p, *q, *leftover;
	struct	mblock *vmemlist;
	vmemlist = proctab[currpid].vmemlist;
	kprintf("&vmemlist=%x, vmemlist=%x, &vmemlist->mnext=%x, vmemlist->mnext=%x, vmemlist->mlen=%d\n",
		&vmemlist,vmemlist,&vmemlist->mnext,vmemlist->mnext,vmemlist->mlen);
	kprintf("\n**\nvmemlist->mnext->mlen = %d\n",vmemlist->mnext->mlen);
	//disable(ps);
	if (nbytes == 0 || nbytes > vmemlist->mnext->mlen || vmemlist->mnext == (struct mblock *) NULL) {
		kprintf("ERROR in vgetmem, nbytes required is %d, pages is backing store is %d\n",nbytes, vmemlist->mlen);
		restore(ps);
		return( (WORD *)SYSERR);
	}
	nbytes = (unsigned int) roundmb(nbytes);
	kprintf("nbtes=%d\n",nbytes);
	int i;
	for (i = 0, q= vmemlist,p=vmemlist->mnext ;
	     p != (struct mblock *) NULL ;
	     q=p,p=p->mnext){

		kprintf("i=%d, q=%x, \t p=%x\n",i,q,p);
		kprintf("p->mlen=%d, \t p=%x\n",p->mlen,p);

		if ( p->mlen == nbytes) {
			q->mnext = p->mnext;
			kprintf("nbytes required is equal to the pages in backing store, which is %d\n", nbytes);
			restore(ps);
			return( (WORD *)p );
		} 
		else if ( p->mlen > nbytes ) {
			kprintf("q=%8x \t q->mnext=%8x \t q->mlen=%d \n&q=%8x \t &q->mnext=%8x \t &(q->mlen)=%8x\np=%8x \t\t p->mnext=%8x \t\t p->mlen=%d \n&p=%8x \t &p->mnext=%8x \t &(p->mlen)=%8x\n",
			q,q->mnext,q->mlen,
			&q,&(q->mnext),&(q->mlen),
			p,p->mnext,p->mlen,
			&p,&(p->mnext),&(p->mlen));

			leftover = (struct mblock *)( (unsigned)p + nbytes );
			q->mnext = leftover;
			leftover->mnext = p->mnext;
			leftover->mlen = p->mlen - nbytes;
			kprintf("&leftover->mnext=%8x \t &leftover->mlen=%8x\n",&leftover->mnext,&leftover->mlen);
			kprintf("leftover=%8x \t leftover->mnext=%8x \t leftover->mlen=%d\nq=%8x \t q->mnext=%8x \t q->mlen=%d \np=%8x \t p->mnext=%8x \t p->mlen=%d \n",
			leftover,leftover->mnext,leftover->mlen,
			q,q->mnext,q->mlen,p,
			p->mnext,p->mlen);

			restore(ps);
			// p is the memory we got after calling vget
			return( (WORD *)p );
		}
	}
		
	restore(ps);
	return( (WORD *)SYSERR );

}


