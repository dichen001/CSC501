#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */

	STATWORD ps; 
	disable(ps);
	// no need to consider whether it's private or not. It must be a NEW UNMAPPED one.
	if(npages<=0 || npages>NBSPG || bsm_tab[bs_id].bs_status == BSM_MAPPED){
		kprintf("ERROR: npages=%d, bsm_tab[bs_id].pid=%d\n",npages,bsm_tab[bs_id].private, bsm_tab[bs_id].bs_pid);
		return SYSERR;
	}
	else{
		curr_bs_num += 1;
		kprintf("Backing_Store[%d] got, total bs in use is %d\n",bs_id, curr_bs_num);
		
		bsm_map(currpid, 4096, bs_id, npages);
		kprintf("get_bs(bs_id(%d), npages(%d)) completed\n",bs_id,npages);
		
		restore(ps);
		return( npages );
	}

}


