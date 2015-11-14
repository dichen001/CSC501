/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{	
	
	if(hsize > NPGPBS){
		kprintf("hsize should be smaller than 128.\n");
		return SYSERR;
	}
	
	STATWORD ps;
	disable(ps);
	
	int pid = create(procaddr, ssize, priority, name, nargs, args); //set stack as private is optional
	
	/* for virtual address mapping */
	int store;
	if( store=get_bsm() == SYSERR)
	{
		kprintf("FULL! Cannot allocate bs for vcreate!\n");
		restore(ps);
		return SYSERR;
	}
	
	proctab[pid].vhpno = 4096;	
	proctab[pid].vhpnpages = hsize;	
	proctab[pid].nframes = 0;
	proctab[pid].bsmap[store].private = BSM_PRIVATE;
	bsm_tab[store].private = BSM_PRIVATE;
	bsm_map(pid, 4096, store, hsize);
	
	kprintf("process[%d] maps its vp(%d-%d) to bs[%d]\n",pid,4096,4096+hsize,store);
	
	/**
	 * two things need notice.   
	 * getmem() is very important here, it allocate memory in kernel to store .vmemlist
	 * sizeof(struct mblock *)= 4; sizeof(struct mblock)=8.
	 */
	proctab[pid].vmemlist = getmem(sizeof(struct mblock *));
	proctab[pid].vmemlist->mnext = (struct mblock *) roundmb(vp2pa(4096));
	proctab[pid].vmemlist->mlen = 0;	
	kprintf("proctab[pid].vmemlist=%x, \tproctab[pid].vmemlist->mnext=%x\n",proctab[pid].vmemlist,proctab[pid].vmemlist->mnext);
	kprintf("vmemlist->mnext(%x) mapping to bs[%d]@(%x), length is %d\n",proctab[pid].vmemlist->mnext, store, bsid2pa(store), hsize);
	
	/**
	 * Since this process hasn't started to run yet page fault won't
	 * work. We need to actually write mnext and mlen directly to the
	 * first 8 bytes of the first page of the backing store, so that 
	 * after page fault, they can get these data through mapping from 
	 * virtual page to physical page.
	 */
    struct mblock * memblock = bsid2pa(store);
    memblock->mnext = 0;  
    memblock->mlen  = hsize*NBPG;
    kprintf("The 'mnext' and 'mlengh' have been writeen to (%x) (%x) of backing store[%d]\n",
    	&(memblock->mnext),&(memblock->mlen),store);


	restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
