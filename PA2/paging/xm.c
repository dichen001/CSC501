/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
  STATWORD ps;
  // Disable interrupts
  disable(ps);

  if(npages > bsm_tab[source].bs_npages)
  {
    kprintf("the bs[%d] has already been occupied by proc[%d]\n",source, bsm_tab[source].bs_pid);
    restore(ps);
    return SYSERR;
  }

  /* sanity check ! */
  if ( (virtpage < 4096) || ( source < 0 ) || ( source >= NBS) ||(npages < 1) || ( npages >NPGPBS)){
  	kprintf("xmmap call error: parameter error! \n");
    restore(ps);
  	return SYSERR;
  }


  // Make sure the bs has been allocated
  if (bsm_tab[source].bs_status == BSM_UNMAPPED) {
      kprintf("xmmap(): ERROR, BS %d is free!\n", source);
      restore(ps);
      return SYSERR;
  }

  // Make sure the bs  is not being used for virtual heap
  if (bsm_tab[source].private == BSM_PRIVATE) {
      kprintf("xmmap(): ERROR, BS %d is used as vheap!\n", source);
      kprintf("xmmap - could not create mapping!\n");
      restore(ps);
      return SYSERR;
  }

  // Add a mapping to the backing store mapping table
  if ( bsm_map(currpid, virtpage, source, npages) == SYSERR) {
      kprintf("xmmap - could not create mapping!\n");
      restore(ps);
      return SYSERR;
  }

  restore(ps);
  return OK;
}


/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage )
{
  STATWORD ps;
  int *store, *pageth;
  unsigned long vaddr;
  /* sanity check ! */
  if ( (virtpage < 4096) ){ 
    	kprintf("xmummap call error: virtpage (%d) invalid! \n", virtpage);
    	return SYSERR;
  }

  // Disable interrupts
  disable(ps);
  pageth = getmem( sizeof(int *) );
  store = getmem( sizeof(int *) );
  if(bsm_lookup(currpid, virtpage, store, pageth) == SYSERR){
      kprintf("xmunmap(): could not find mapping!\n");
      restore(ps);
      return SYSERR;
  }
  //decrease the ref of frames used by this bs and free them if ref == 0.
  if(GDB)
    kprintf("in xmunmap(), pid=%d\n",currpid);
  dec_frm_refcnt(currpid, *store);
  bsm_tab[*store].mapping_num -= 1;
  // Finally must invalidate TLB entries since page table contents 
  // have changed. From intel vol III
  //
  // All of the (nonglobal) TLBs are automatically invalidated any
  // time the CR3 register is loaded.
  set_PDBR(currpid);
  restore(ps);
  return OK;
}

