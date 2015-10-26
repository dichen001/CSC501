/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

bs_map_t bsm_tab[NBS];

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
	if(bsm_tab[store].private == BSM_PRIVATE){
		kprintf("!!! This bs is private, cannot be remapped. !!!\n");
		return SYSERR;
	}
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
	/**
	 * ****************************************************************************************************************************************************
	 * 	stopped here....  To be continue for coding vreate.c 
	 * ****************************************************************************************************************************************************
	 */
	proctab[pid].vmemlist = NULL;	
	proctab[pid].vmemlist = getmem(sizeof(struct mblock));
	proctab[pid].vmemlist -> mlen = npages * NBPG;
	proctab[pid].vmemlist -> mnext = NULL;	
	kprintf("Backing_Store[%d] mapping updated\n",store);
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
			kprintf("Backing_Store[%d] got\n",i);
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
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}


