/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

bs_map_t bsm_tab[NBS];
int curr_bs_num;

/*-------------------------------------------------------------------------
 * init_bsm- initialize backing store map table
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{	kprintf("initialize backing store map table\n");
	int i;
	for (i = 0; i < NBS; ++i)
	{
		bsm_tab[i].mapping_num = 0;
		bsm_tab[i].private = BSM_NOTPRIVATE;
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].bs_npages =	-1;
		bsm_tab[i].bs_sem =	-1;
	}
	curr_bs_num = 0;
	kprintf("backing store map table initialized \n");
	return OK;
}
 
/*-------------------------------------------------------------------------
 * update_bsm- update the mapping of backing store from proces's vpage.
 *-------------------------------------------------------------------------
 */
SYSCALL update_bsm( int pid, int type, int vhpno, int npages, int store )
{	kprintf("update backing store map table {pid: %d(%s), vpage: %d, store: %d, npages: %d }\n",
										pid, proctab[pid].pname, vhpno, store, npages);
	bsm_tab[store].mapping_num += 1;
	bsm_tab[store].private = type;
	bsm_tab[store].bs_status = BSM_MAPPED;
	bsm_tab[store].bs_pid = pid;
	bsm_tab[store].bs_vpno = vhpno;
	bsm_tab[store].bs_npages =	npages;
	//bsm_tab[store].bs_sem =	-1;
	
	proctab[pid].store = store;
	proctab[pid].vhpno = vhpno;
	proctab[pid].vhpnpages = npages;
	//initialize the virtual memory list, and map it to the according backing store.
	struct	mblock	*mptr;
	//kprintf("if don't use getmem() here, it will probably go wrong.\n");  //it turns out everything goes well.
	kprintf("sizeof(struct mblock *)=%d,\t sizeof(struct mblock)=%d\n",sizeof(struct mblock *),sizeof(struct mblock));
	proctab[pid].vmemlist = getmem(sizeof(struct mblock *));
	proctab[pid].vmemlist->mnext = mptr = (struct mblock *) roundmb(bsid2pa(store));
	mptr->mnext = 0;
	mptr->mlen = npages * NBPG;		//attention, this is in bytes.
	kprintf("Backing_Store[%d] mapping updated\n",store);
	kprintf("vmemlist->mnext mapping to bs[%d] @ %x, length is %d\n",store,bsid2pa(store), npages);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm()
{	int i;
	for (i = 0; i < NBS; ++i)
	{
		if(bsm_tab[i].bs_status == BSM_UNMAPPED){
			curr_bs_num += 1;
			kprintf("Backing_Store[%d] got, total bs in use is %d\n",i, curr_bs_num);
			return i;
		}			
	}
	kprintf("bsmtab[] is full \n");
	return -1;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	kprintf("before free_bsm(%d), its mapping_num = %d \n", i, bsm_tab[i].mapping_num);
	bsm_tab[i].mapping_num = 0;
	bsm_tab[i].private = BSM_NOTPRIVATE;
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	bsm_tab[i].bs_pid = -1;
	bsm_tab[i].bs_vpno = -1;
	bsm_tab[i].bs_npages =	-1;
	bsm_tab[i].bs_sem =	-1;

	proctab[currpid].store = -1;
	proctab[currpid].vhpno = -1;
	proctab[currpid].vhpnpages = -1;
	proctab[currpid].vmemlist = NULL;
	kprintf("backing_store[%d] released by process: %s\n",i,proctab[currpid].pname);
	kprintf("free_bsm(%d) completed\n", i);
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int store, int npages)
{
	kprintf("bsm_map {pid: %d(%s), vpage: %d, store: %d, npages: %d }\n",
				pid, proctab[pid].pname, vpno, store, npages);
	//bsm_tab[] serves to show the globla value of a backing store.
	bsm_tab[store].mapping_num += 1;
	bsm_tab[store].bs_status = BSM_MAPPED;
	if(bsm_tab[store].mapping_num == 1){
		bsm_tab[store].bs_pid = pid;
		bsm_tab[store].bs_vpno = vpno;
		bsm_tab[store].bs_npages =	npages;
		//bsm_tab[store].bs_sem =	-1;	
	}
	
	//proctab[pid].xxx serves to show the process specific value of a backing store.
	/**
	 * the proctab[pid].bsmap  need to be implement.
	 */
	proctab[pid].store = store;
	proctab[pid].vhpno = vpno;
	proctab[pid].vhpnpages = npages;
	
	/**
	 * two things need notice.   
	 * getmem() is very important here, it allocate memory in kernel to store .vmemlist
	 * sizeof(struct mblock *)= 4; sizeof(struct mblock)=8.
	 */
	proctab[pid].vmemlist = getmem(sizeof(struct mblock *));
	proctab[pid].vmemlist->mnext = (struct mblock *) roundmb(vp2pa(vpno));
	proctab[pid].vmemlist->mlen = vp2pa(npages);

	kprintf("Backing_Store[%d] mapping updated\n",store);
	kprintf("proctab[pid].vmemlist=%x, \tproctab[pid].vmemlist->mnext=%x\n",proctab[pid].vmemlist,proctab[pid].vmemlist->mnext);
	kprintf("vmemlist->mnext(%x) mapping to bs[%d]@(%x), length is %d\n",proctab[pid].vmemlist->mnext,store,bsid2pa(store), npages);
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}


