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
 * update_bsm- update the bsm when process[pid] map a backing store.
 *-------------------------------------------------------------------------
 */
SYSCALL update_bsm(int pid, int type)
{	kprintf("initialize backing store map table for process %s \n",proctab[pid].pname);
	int i;
	for (i = 0; i < NBS; ++i)
	{
		bsm_tab[i].mapping_num = 1;
		bsm_tab[i].private = BSM_NOTPRIVATE;
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = -1;
		bsm_tab[i].bs_npages =	-1;
		bsm_tab[i].bs_sem =	-1;
	}
	kprintf("backing store initialized \n");
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
			kprintf("get bsmtab[%d] \n",i);
			return OK;
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


