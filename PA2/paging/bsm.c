/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

int curr_bs_num;

/*-------------------------------------------------------------------------
 * init_bsm- initialize backing store map table
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm(bs_map_t *bs)
{	kprintf("initialize backing store map table\n");
	int i;
	for (i = 0; i < NBS; ++i)
	{
		bs[i].mapping_num = 0;
		bs[i].private = BSM_NOTPRIVATE;
		bs[i].frames = NULL;

		bs[i].bs_status = BSM_UNMAPPED;
		bs[i].bs_pid = -1;
		bs[i].bs_vpno = 0;
		bs[i].bs_npages = 0;
		bs[i].bs_sem =	-1;
	}
	curr_bs_num = 0;
	kprintf("backing store map table initialized \n");
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
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	//not complete, wait to be implemented.
	kprintf("before free_bsm(%d), its mapping_num = %d \n", i, bsm_tab[i].mapping_num);
	bsm_tab[i].mapping_num = 0;
	bsm_tab[i].private = BSM_NOTPRIVATE;
	bsm_tab[i].frames = NULL;

	bsm_tab[i].bs_status = BSM_UNMAPPED;
	bsm_tab[i].bs_pid = -1;
	bsm_tab[i].bs_vpno = 0;
	bsm_tab[i].bs_npages =	0;
	bsm_tab[i].bs_sem =	-1;

	proctab[currpid].bsmap[i].mapping_num = 0;
	proctab[currpid].bsmap[i].private = BSM_NOTPRIVATE;
	proctab[currpid].bsmap[i].frames = NULL;
	
	proctab[currpid].bsmap[i].bs_status = BSM_UNMAPPED;
	proctab[currpid].bsmap[i].bs_pid = -1;
	proctab[currpid].bsmap[i].bs_vpno = -1;
	proctab[currpid].bsmap[i].bs_npages =	-1;
	proctab[currpid].bsmap[i].bs_sem =	-1;

	proctab[currpid].vmemlist = NULL;
	kprintf("bs_id=%d, bs.status=%d, bs.private=%d, bs.pid=%d, proc.bs.private=%d, proc.bs.status=%d\n",
			i, bsm_tab[i].bs_status, bsm_tab[i].private, bsm_tab[i].bs_pid, proctab[currpid].bsmap[i].private, proctab[currpid].bsmap[i].bs_status);
	kprintf("backing_store[%d] released by process: %s\n",i,proctab[currpid].pname);
	kprintf("free_bsm(%d) completed\n", i);
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, unsigned int vpno, int* store, int* pageth)
{	
	int i;
	kprintf("looking up bs for {pid=%d, vpno=%d}\n",pid,vpno);
	for(i = 0; i < NBS; i ++){
        bs_map_t *bsptr = &proctab[pid].bsmap[i];
        if(bsptr->bs_status == BSM_UNMAPPED)
        	continue;
        if((vpno >= bsptr->bs_vpno) && (vpno < bsptr->bs_vpno + bsptr->bs_npages))
        {	
        	kprintf("store=%d, vpno=%d, bs_vpno=%d, bs_napges=%d\n",i,vpno,bsptr->bs_vpno,bsptr->bs_npages);
	        *store = i;
	        *pageth = vpno - bsptr->bs_vpno;
			kprintf("maping found: {pid: %d, vpno: %d, store: %d, pageth: %d}\n",pid,vpno,*store,*pageth);
	        return OK;
        }
    }
    kprintf("No maping found. pid=%d vpno=%d\n",pid,vpno);
	*store = -1;
	*pageth = -1;
    return SYSERR;
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
	kprintf("mapping on backing_store[%d]= %d\n",store,bsm_tab[store].mapping_num);
	//save the data of the first process that maps this bs.
	if(bsm_tab[store].mapping_num == 1){
		bsm_tab[store].bs_status = BSM_MAPPED;
		bsm_tab[store].bs_pid = pid;
		bsm_tab[store].bs_vpno = vpno;
		bsm_tab[store].bs_npages =	npages;
		//bsm_tab[store].bs_sem =	-1;	
	}
	proctab[pid].bsmap[store].bs_status = BSM_MAPPED;
	proctab[pid].bsmap[store].bs_vpno = vpno;
	proctab[pid].bsmap[store].bs_npages = npages;
	kprintf("Backing_Store[%d] mapping updated\n",store);
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
}


